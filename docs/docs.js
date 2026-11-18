(() => {
  'use strict';

  const FILE_BASE='https://github.com/mippi-the-dork/Focus/blob/main/docs/';
  const $=s=>document.querySelector(s);
  const els={
    nav:$('#nav'), article:$('#article'), crumbs:$('#crumbs'), toc:$('#toc'),
    pager:$('#pager'), source:$('#source'), kind:$('#kind-switch'),
    search:$('#search'), results:$('#results'), menu:$('#menu'), sidebar:$('#sidebar'),
    scrim:$('#scrim'), fatal:$('#fatal'), fatalCopy:$('#fatal-copy'), version:$('#docs-version')
  };

  const state={
    manifest:null,
    categories:[],
    pages:[],
    byPath:new Map(),
    current:'README.md',
    currentMeta:null,
    request:0,
    searchCursor:-1,
    converter:null,
    searchIndexPromise:null,
    searchIndexReady:false
  };

  const esc=v=>String(v??'')
    .replaceAll('&','&amp;').replaceAll('<','&lt;').replaceAll('>','&gt;')
    .replaceAll('"','&quot;').replaceAll("'","&#39;");

  function normalize(path){
    const out=[];
    String(path||'').replaceAll('\\','/').split('/').forEach(part=>{
      if(!part||part==='.') return;
      if(part==='..'){ out.pop(); return; }
      out.push(part);
    });
    return out.join('/');
  }

  // Redirect published bookmarks from the original category layout.
  const legacyRoutes = {
    '02-visual-and-interaction/visibility/': '02-quick-actions/visual/',
    '02-visual-and-interaction/solo/': '02-quick-actions/visual/',
    '02-visual-and-interaction/locks/': '02-quick-actions/interaction/',
    '02-visual-and-interaction/hierarchies/': '04-workflows-and-implementation/workflows/',
    '03-quick-properties/simulation/': '02-quick-actions/simulation/',
    '03-quick-properties/lighting/': '02-quick-actions/lighting/',
    '03-quick-properties/network/': '02-quick-actions/network/',
    '03-quick-properties/mobility/': '02-quick-actions/mobility/',
    '03-quick-properties/eligibility/': '04-workflows-and-implementation/reference/',
    '04-settings-and-saving/': '03-settings-and-saving/',
    '05-implementation/': '04-workflows-and-implementation/',
    '06-support/': '05-support-and-troubleshooting/'
  };
  function canonicalPath(path){
    const normalized = normalize(path);
    if (normalized === '02-visual-and-interaction/README.md' || normalized === '03-quick-properties/README.md' ||
        normalized.startsWith('03-quick-properties/eligibility/') ||
        normalized.startsWith('05-implementation/reference/'))
      return '02-quick-actions/README.md';
    for (const [oldPrefix,newPrefix] of Object.entries(legacyRoutes)){
      if (normalized.startsWith(oldPrefix)) return newPrefix+normalized.slice(oldPrefix.length);
    }
    if (normalized === '04-settings-and-saving/README.md') return '03-settings-and-saving/README.md';
    if (normalized === '05-implementation/README.md') return '04-workflows-and-implementation/README.md';
    if (normalized === '06-support/README.md') return '05-support-and-troubleshooting/README.md';
    return normalized;
  }

  function splitHash(value){
    const i=value.indexOf('#');
    return i<0?[value,'']:[value.slice(0,i),value.slice(i+1)];
  }

  function resolveRelative(current,href){
    const [target,hash]=splitHash(href);
    const base=current.includes('/')?current.slice(0,current.lastIndexOf('/')+1):'';
    return {path:normalize(base+target),hash};
  }

  function pageUrl(path,hash=''){
    const u=new URL(location.href);
    u.search='';
    u.hash='';
    u.searchParams.set('page',path);
    if(hash) u.hash=hash;
    return u.pathname+'?'+u.searchParams.toString()+u.hash;
  }

  function currentFromUrl(){
    const raw=new URLSearchParams(location.search).get('page')||'README.md';
    const path=canonicalPath(raw);
    return path&& !path.startsWith('../') ? path : 'README.md';
  }

  function parseCategories(md){
    const out=[];
    const re=/^\|\s*\[(\d{2})\.\s+(.+?)\]\(([^)]+\/README\.md)\)\s*\|\s*(\d+)\s*\|\s*(.*?)\s*\|\s*$/gm;
    let m;
    while((m=re.exec(md))){
      const overview=normalize(m[3].trim());
      out.push({
        number:m[1],
        title:m[2].trim(),
        overview,
        dir:overview.replace(/\/README\.md$/i,''),
        expected:Number(m[4])||0,
        summary:m[5].trim(),
        features:[]
      });
    }
    return out;
  }

  function parseFeatures(md,cat){
    const out=[];
    const re=/^\|\s*\*\*(.+?)\*\*\s*\|\s*(.*?)\s*\|\s*\[Design\]\(([^)]+)\)\s*\|\s*\[Technical\]\(([^)]+)\)\s*\|\s*$/gm;
    let m;
    while((m=re.exec(md))){
      const design=normalize(cat.dir+'/'+m[3].trim());
      const technical=normalize(cat.dir+'/'+m[4].trim());
      out.push({
        title:m[1].trim(),
        summary:m[2].trim(),
        design,
        technical,
        slug:design.split('/').slice(-2,-1)[0]||''
      });
    }
    return out;
  }

  async function loadStructure(){
    const home=await fetch(state.manifest.home,{cache:'no-cache'});
    if(!home.ok) throw new Error('Could not load documentation home.');
    state.categories=parseCategories(await home.text());
    if(!state.categories.length) throw new Error('No numbered documentation categories were found.');

    await Promise.all(state.categories.map(async cat=>{
      const r=await fetch(cat.overview,{cache:'no-cache'});
      if(!r.ok) throw new Error('Could not load '+cat.overview);
      cat.features=parseFeatures(await r.text(),cat);
      if(cat.expected && cat.features.length!==cat.expected){
        console.warn('Documentation count mismatch for',cat.dir,'expected',cat.expected,'found',cat.features.length);
      }
    }));
  }

  function label(p){
    if(!p) return 'Documentation';
    return p.kind==='Design'||p.kind==='Technical' ? p.feature+' - '+p.kind : p.title;
  }

  function buildPages(){
    const pages=[{
      title:'Documentation Home',path:state.manifest.home,kind:'Home',
      summary:'Documentation home and category index.',search:'documentation home overview index'
    }];

    state.categories.forEach(cat=>{
      pages.push({
        title:cat.number+'. '+cat.title,path:cat.overview,kind:'Category',
        category:cat.title,categoryNumber:cat.number,categoryDir:cat.dir,
        summary:cat.summary,search:cat.number+' '+cat.title+' '+cat.summary
      });
      cat.features.forEach(f=>{
        pages.push({
          title:f.title,feature:f.title,path:f.design,sibling:f.technical,kind:'Design',
          category:cat.title,categoryNumber:cat.number,categoryDir:cat.dir,
          summary:f.summary,search:cat.number+' '+cat.title+' '+f.title+' design '+f.summary
        });
        pages.push({
          title:f.title,feature:f.title,path:f.technical,sibling:f.design,kind:'Technical',
          category:cat.title,categoryNumber:cat.number,categoryDir:cat.dir,
          summary:f.summary,search:cat.number+' '+cat.title+' '+f.title+' technical implementation '+f.summary
        });
      });
    });

    pages.push({title:'Roadmap',path:'ROADMAP.md',kind:'Document',
      summary:'Planned Quick Action categories, third-party extension API and Quick Widget Builder',
      search:'roadmap planned features future extension api third party plugin micro widget builder'});
    pages.push({title:'Documentation Audit',path:'DOCUMENTATION_AUDIT.md',kind:'Document',
      summary:'Documentation quality findings, prioritized fixes and verification gaps',
      search:'audit documentation improvements quality gaps testing screenshots search'});
    pages.push({title:'Verification Matrix',path:'VERIFICATION_MATRIX.md',kind:'Document',
      summary:'Native Unreal Editor and website QA checklists with explicit results',
      search:'verification matrix qa tests perforce one file per actor world partition focus website'});
    state.pages=pages;
    state.byPath=new Map(pages.map(p=>[p.path,p]));
  }

  function renderNav(){
    const out=[];
    out.push('<a class="nav-home" data-page="'+esc(state.manifest.home)+'" href="'+pageUrl(state.manifest.home)+'">⌂ Documentation Home</a>');
    out.push('<a class="ref-link" data-page="ROADMAP.md" href="'+pageUrl('ROADMAP.md')+'">Roadmap · Planned Features</a>');
    out.push('<div class="nav-label">CURRENT VERSION · '+esc((state.manifest.baseline||'v2.1.0').replace('Focus ','').toUpperCase())+'</div>');
    
    
    
    out.push('<div class="nav-label">FEATURE DOCUMENTATION</div>');

    state.categories.forEach(cat=>{
      out.push(
        '<details class="nav-category" data-category="'+esc(cat.dir)+'">'+
        '<summary><span class="cat-num">'+esc(cat.number)+'</span><span class="cat-title">'+esc(cat.title)+'</span><span class="cat-arrow">›</span></summary>'+
        '<div class="cat-body">'+
        '<a class="cat-overview" data-page="'+esc(cat.overview)+'" href="'+pageUrl(cat.overview)+'">Overview</a>'
      );
      cat.features.forEach(f=>{
        out.push(
          '<div class="feature-row">'+
          '<a class="feature" data-page="'+esc(f.design)+'" href="'+pageUrl(f.design)+'" title="'+esc(f.summary)+'">'+esc(f.title)+'</a>'+
          '<a class="mini" data-page="'+esc(f.design)+'" href="'+pageUrl(f.design)+'" title="Design">D</a>'+
          '<a class="mini" data-page="'+esc(f.technical)+'" href="'+pageUrl(f.technical)+'" title="Technical">T</a>'+
          '</div>'
        );
      });
      out.push('</div></details>');
    });

    els.nav.innerHTML=out.join('');
  }

  function syncNav(){
    els.nav.querySelectorAll('.current').forEach(n=>n.classList.remove('current'));
    els.nav.querySelectorAll('[data-page]').forEach(n=>{
      if(n.dataset.page===state.current) n.classList.add('current');
    });
    els.nav.querySelectorAll('.nav-category').forEach(d=>{
      const match=state.current.startsWith(d.dataset.category+'/');
      d.classList.toggle('current',match);
      if(match) d.open=true;
    });
  }

  function renderCrumbs(meta){
    const out=['<a data-page="README.md" href="'+pageUrl('README.md')+'">HOME</a>'];
    if(meta?.categoryDir){
      const cat=state.categories.find(x=>x.dir===meta.categoryDir);
      if(cat){
        out.push('<span class="sep">/</span>');
        out.push('<a data-page="'+esc(cat.overview)+'" href="'+pageUrl(cat.overview)+'">'+esc(cat.number+' '+cat.title.toUpperCase())+'</a>');
      }
    }
    if(meta?.feature){
      out.push('<span class="sep">/</span><span>'+esc(meta.feature.toUpperCase())+'</span>');
    }
    if(meta && !['Home','Category'].includes(meta.kind)){
      out.push('<span class="sep">/</span><span class="current-crumb">'+esc(meta.kind.toUpperCase())+'</span>');
    }
    els.crumbs.innerHTML=out.join('');
  }

  function renderKind(meta){
    if(!meta?.sibling || !['Design','Technical'].includes(meta.kind)){
      els.kind.hidden=true; els.kind.innerHTML=''; return;
    }
    const design=meta.kind==='Design'?meta.path:meta.sibling;
    const technical=meta.kind==='Technical'?meta.path:meta.sibling;
    els.kind.hidden=false;
    els.kind.innerHTML=
      '<a class="kind-link '+(meta.kind==='Design'?'active':'')+'" data-page="'+esc(design)+'" href="'+pageUrl(design)+'">DESIGN</a>'+
      '<a class="kind-link '+(meta.kind==='Technical'?'active':'')+'" data-page="'+esc(technical)+'" href="'+pageUrl(technical)+'">TECHNICAL</a>';
  }

  function slug(text,used){
    let base=String(text).toLowerCase().replace(/<[^>]*>/g,'').replace(/&[a-z0-9#]+;/g,' ')
      .replace(/[^a-z0-9]+/g,'-').replace(/^-+|-+$/g,'')||'section';
    let s=base,i=2;
    while(used.has(s)) s=base+'-'+i++;
    used.add(s);
    return s;
  }

  function enhance(){
    const used=new Set();
    els.article.querySelectorAll('h1,h2,h3,h4').forEach(h=>{
      h.id=slug(h.textContent,used);
      if(h.tagName!=='H1'){
        const a=document.createElement('a');
        a.className='heading-anchor'; a.href='#'+h.id; a.textContent='#';
        a.setAttribute('aria-label','Link to '+h.textContent);
        h.prepend(a);
      }
    });

    // Images in Markdown are resolved relative to the Markdown source file,
    // rather than the site's query-driven /Focus/ page URL. External images
    // are left unchanged; this also keeps GitHub's Markdown preview correct.
    els.article.querySelectorAll('img[src]').forEach(img=>{
      const raw=img.getAttribute('src')||'';
      if(!raw||/^(?:[a-z]+:|\/|#)/i.test(raw))return;
      const target=resolveRelative(state.current,raw);
      if(target.path)img.setAttribute('src',target.path+(target.hash||''));
    });

    els.article.querySelectorAll('table').forEach(table=>{
      if(table.parentElement?.classList.contains('table-wrap')) return;
      const wrap=document.createElement('div'); wrap.className='table-wrap';
      table.parentNode.insertBefore(wrap,table); wrap.appendChild(table);
    });

    els.article.querySelectorAll('pre').forEach(pre=>{
      const code=pre.querySelector('code'); if(!code) return;
      const b=document.createElement('button'); b.type='button'; b.className='copy'; b.textContent='COPY';
      b.addEventListener('click',async()=>{
        try{await navigator.clipboard.writeText(code.textContent||'');b.textContent='COPIED';}
        catch{b.textContent='FAILED';}
        setTimeout(()=>b.textContent='COPY',1000);
      });
      pre.appendChild(b);
    });

    els.article.querySelectorAll('a[href]').forEach(a=>{
      if(a.classList.contains('heading-anchor')) return;
      const raw=a.getAttribute('href')||'';
      if(/^(https?:|mailto:|tel:)/i.test(raw)){
        a.target='_blank';a.rel='noopener noreferrer';return;
      }
      if(raw.startsWith('#')) return;
      const x=resolveRelative(state.current,raw);
      if(/\.md$/i.test(x.path)){
        a.href=pageUrl(x.path,x.hash);a.dataset.page=x.path;if(x.hash)a.dataset.hash=x.hash;
      }
    });
  }

  function renderToc(){
    const heads=[...els.article.querySelectorAll('h2,h3')];
    els.toc.innerHTML=heads.length?heads.map(h=>
      '<a data-depth="'+(h.tagName==='H3'?'3':'2')+'" href="#'+esc(h.id)+'">'+esc(h.textContent.replace(/^#/,'').trim())+'</a>'
    ).join(''):'<span class="search-empty">No page sections.</span>';
  }

  function renderMarkdown(md){
    let html;
    if(window.showdown?.Converter){
      state.converter ||= new window.showdown.Converter({
        tables:true,strikethrough:true,tasklists:true,ghCodeBlocks:true,
        simplifiedAutoLink:true,literalMidWordUnderscores:true,openLinksInNewWindow:false
      });
      html=state.converter.makeHtml(md);
    }else{
      html='<pre><code>'+esc(md)+'</code></pre>';
    }
    if(window.DOMPurify?.sanitize) html=window.DOMPurify.sanitize(html,{ADD_ATTR:['target','rel']});
    els.article.innerHTML=html;
    enhance();
    renderToc();
  }

  function renderPager(){
    const i=state.pages.findIndex(p=>p.path===state.current);
    const prev=i>0?state.pages[i-1]:null;
    const next=i>=0&&i<state.pages.length-1?state.pages[i+1]:null;
    els.pager.innerHTML=
      (prev?'<a class="pager-link" data-page="'+esc(prev.path)+'" href="'+pageUrl(prev.path)+'"><small>← PREVIOUS</small><strong>'+esc(label(prev))+'</strong></a>':'<span></span>')+
      (next?'<a class="pager-link next" data-page="'+esc(next.path)+'" href="'+pageUrl(next.path)+'"><small>NEXT →</small><strong>'+esc(label(next))+'</strong></a>':'');
  }

  async function loadPage(path,{push=false,hash=''}={}){
    const token=++state.request;
    const normalized=canonicalPath(path||'README.md')||'README.md';
    state.current=normalized;
    state.currentMeta=state.byPath.get(normalized)||{title:normalized.split('/').pop(),path:normalized,kind:'Document'};

    if(push) history.pushState({page:normalized},'',pageUrl(normalized,hash));

    syncNav();
    renderCrumbs(state.currentMeta);
    renderKind(state.currentMeta);
    renderPager();
    els.source.href=FILE_BASE+normalized.split('/').map(encodeURIComponent).join('/');
    els.article.innerHTML='<div class="loading">LOADING DOCUMENTATION</div>';
    els.toc.innerHTML='';

    try{
      const r=await fetch(normalized,{cache:'no-cache'});
      if(!r.ok) throw new Error('HTTP '+r.status+' loading '+normalized);
      const md=await r.text();
      if(token!==state.request) return;
      renderMarkdown(md);
      document.title=label(state.currentMeta)+' | Focus Documentation';
      const target=hash||location.hash.slice(1);
      if(target) requestAnimationFrame(()=>document.getElementById(target)?.scrollIntoView({block:'start'}));
      else window.scrollTo({top:0,behavior:'auto'});
      closeNav();
    }catch(error){
      if(token!==state.request) return;
      els.article.innerHTML='<div class="error">FAILED TO LOAD '+esc(normalized)+'<br>'+esc(error.message)+'</div>';
    }
  }

  function closeNav(){
    document.body.classList.remove('nav-open');
    els.menu?.setAttribute('aria-expanded','false');
    if(els.scrim) els.scrim.hidden=true;
  }
  function toggleNav(){
    const open=!document.body.classList.contains('nav-open');
    document.body.classList.toggle('nav-open',open);
    els.menu?.setAttribute('aria-expanded',String(open));
    if(els.scrim) els.scrim.hidden=!open;
  }

  // Index page bodies on first search with bounded network concurrency.
  // Title-based results remain available while indexing is in progress.
  function stripSearchText(markdown){
    return String(markdown||'')
      .replace(/!\[([^\]]*)\]\([^)]+\)/g,' $1 ')
      .replace(/\[([^\]]+)\]\([^)]+\)/g,' $1 ')
      .replace(/<[^>]*>/g,' ')
      .replace(/[\x60*_#>|]/g,' ')
      .replace(/\s+/g,' ').trim();
  }

  function buildFullTextIndex(){
    if(state.searchIndexPromise)return state.searchIndexPromise;
    const pages=state.pages.slice();
    let cursor=0;
    async function worker(){
      while(cursor<pages.length){
        const p=pages[cursor++];
        try{
          const r=await fetch(p.path,{cache:'force-cache'});
          if(!r.ok)continue;
          p.searchBody=stripSearchText(await r.text());
          p.searchBodyLower=p.searchBody.toLowerCase();
        }catch(error){
          console.warn('Focus search could not index',p.path,error);
        }
      }
    }
    state.searchIndexPromise=Promise.all(Array.from({length:Math.min(5,pages.length)},()=>worker()))
      .then(()=>{
        state.searchIndexReady=true;
        if(els.search.value.trim())renderSearch();
      });
    return state.searchIndexPromise;
  }

  function searchExcerpt(page,terms){
    const body=page.searchBody||'';
    if(!body)return page.summary||page.category||'';
    const lower=page.searchBodyLower||body.toLowerCase();
    let at=-1;
    for(const term of terms){
      const i=lower.indexOf(term);
      if(i>=0&&(at<0||i<at))at=i;
    }
    if(at<0)return (page.summary||body).slice(0,170);
    const start=Math.max(0,at-52),end=Math.min(body.length,at+140);
    return (start?'…':'')+body.slice(start,end)+(end<body.length?'…':'');
  }

  function searchPages(query){
    const q=query.trim().toLowerCase();if(!q)return[];
    const terms=q.split(/\s+/).filter(Boolean);
    return state.pages.map(page=>{
      const title=label(page).toLowerCase();
      const hay=((page.search||'')+' '+page.path).toLowerCase();
      const body=page.searchBodyLower||'';
      let score=0;
      if(title===q)score+=100;
      if(title.startsWith(q))score+=55;
      if(title.includes(q))score+=32;
      if(hay.includes(q))score+=20;
      for(const term of terms){
        if(hay.includes(term))score+=title.includes(term)?12:5;
        else if(body.includes(term))score+=2;
        else return null;
      }
      return {page,score,excerpt:searchExcerpt(page,terms)};
    }).filter(Boolean).sort((a,b)=>b.score-a.score||label(a.page).localeCompare(label(b.page))).slice(0,16);
  }

  function renderSearch(){
    const q=els.search.value,found=searchPages(q);state.searchCursor=-1;
    if(!q.trim()){els.results.hidden=true;els.results.innerHTML='';return;}
    els.results.hidden=false;
    const indexing=!state.searchIndexReady;
    if(!found.length){
      els.results.innerHTML='<div class="search-empty">'+
        (indexing?'Searching page titles; indexing full documentation…':'No matching documentation pages.')+'</div>';
      return;
    }
    els.results.innerHTML=(indexing?'<div class="search-empty">Indexing page content… title matches shown now.</div>':'')+
      found.map(x=>
      '<a class="result" data-page="'+esc(x.page.path)+'" href="'+pageUrl(x.page.path)+'">'+
      '<strong>'+esc(label(x.page))+'</strong><small>'+esc(x.excerpt)+'</small>'+
      '<span class="result-path">'+esc(x.page.path)+'</span></a>'
    ).join('');
  }

  function moveSearch(delta){
    const rows=[...els.results.querySelectorAll('.result')]; if(!rows.length)return;
    state.searchCursor=Math.max(-1,Math.min(rows.length-1,state.searchCursor+delta));
    rows.forEach((r,i)=>r.classList.toggle('active',i===state.searchCursor));
    if(state.searchCursor>=0)rows[state.searchCursor].scrollIntoView({block:'nearest'});
  }

  function activateSearch(){
    const rows=[...els.results.querySelectorAll('.result')];
    const row=rows[state.searchCursor]||rows[0];if(!row)return;
    loadPage(row.dataset.page,{push:true});els.search.value='';renderSearch();els.search.blur();
  }

  function bind(){
    document.addEventListener('click',e=>{
      const link=e.target.closest('[data-page]');
      if(link){
        e.preventDefault();
        loadPage(link.dataset.page,{push:true,hash:link.dataset.hash||''});
        return;
      }
      if(!e.target.closest('.search-wrap'))els.results.hidden=true;
    });
    els.menu?.addEventListener('click',toggleNav);
    els.scrim?.addEventListener('click',closeNav);
    els.search.addEventListener('input',()=>{if(els.search.value.trim())void buildFullTextIndex();renderSearch();});
    els.search.addEventListener('focus',renderSearch);
    els.search.addEventListener('keydown',e=>{
      if(e.key==='ArrowDown'){e.preventDefault();moveSearch(1);}
      else if(e.key==='ArrowUp'){e.preventDefault();moveSearch(-1);}
      else if(e.key==='Enter'&&!els.results.hidden){e.preventDefault();activateSearch();}
      else if(e.key==='Escape'){els.search.value='';renderSearch();els.search.blur();}
    });
    document.addEventListener('keydown',e=>{
      const tag=document.activeElement?.tagName||'';
      if(e.key==='/'&&!e.ctrlKey&&!e.metaKey&&!e.altKey&&!/^(INPUT|TEXTAREA|SELECT)$/.test(tag)){
        e.preventDefault();els.search.focus();
      }
      if((e.ctrlKey||e.metaKey)&&e.key.toLowerCase()==='k'){
        e.preventDefault();els.search.focus();els.search.select();
      }
      if(e.key==='Escape'&&document.body.classList.contains('nav-open'))closeNav();
    });
    addEventListener('popstate',()=>loadPage(currentFromUrl(),{hash:location.hash.slice(1)}));
  }

  async function init(){
    try{
      const r=await fetch('site-manifest.json',{cache:'no-cache'});
      if(!r.ok)throw new Error('Could not load documentation manifest.');
      state.manifest=await r.json();
      const currentVersion=String(state.manifest.baseline||'Focus v2.1.0').match(/v\d+\.\d+(?:\.\d+)?/)?.[0]||'v2.1.0';
      if(els.version) els.version.textContent=currentVersion+' DOCUMENTED';
      await loadStructure();
      buildPages();
      renderNav();
      bind();
      await loadPage(currentFromUrl(),{hash:location.hash.slice(1)});
    }catch(error){
      els.fatal.hidden=false;
      els.fatalCopy.textContent=String(error?.message||error||'Unknown startup error.');
    }
  }

  init();
})();