/* Editor page served at "/": textarea, locale picker, Run/Stop, live output.
   Self-contained — the board is offline, so no external assets.
   Run POSTs the code, then polls /output and appends text as it streams in;
   Stop ends a running program. The default program reads the analog value on
   pin 0 once a second and prints it. */
static const char AKSA_INDEX_HTML[] PROGMEM = R"AKSA(<!doctype html>
<html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Aksa</title>
<style>
body{font-family:system-ui,sans-serif;margin:12px;display:flex;flex-direction:column;gap:8px;height:94vh}
#wrap{flex:1;position:relative;border:2px solid #888;border-radius:8px;overflow:hidden}
#src,#hl{margin:0;border:0;padding:8px;font:16px/1.5 ui-monospace,monospace;white-space:pre-wrap;overflow-wrap:break-word;position:absolute;inset:0;overflow:auto}
#hl{pointer-events:none;background:#fff;color:#333}
#src{background:transparent;color:transparent;caret-color:#111;-webkit-text-fill-color:transparent;resize:none}
.kw{color:#d63384}.st{color:#16a34a}.nu{color:#2563eb}.ct{color:#9ca3af;font-style:italic}
pre{background:#f0f0f0;border-radius:8px;padding:8px;min-height:4em;white-space:pre-wrap;margin:0;overflow:auto}
#out{flex:0 1 auto;max-height:40%}
.row{display:flex;gap:8px}
button{flex:1;font-size:18px;padding:10px;border:0;border-radius:8px;background:#2563eb;color:#fff}
#stop{background:#dc2626}
select{font-size:16px;padding:6px;border-radius:8px}
</style></head><body>
<div id="wrap"><pre id="hl"></pre><textarea id="src" spellcheck="false" autocapitalize="off" autocomplete="off" autocorrect="off">selama (benar) {
  buat data = baca_analog(0)
  cetak(data)
  jika (data > 700) {
    nyalakan(15)
  } lainnya {
    matikan(15)
  }
  tunggu(1)
}</textarea></div>
<div class="row">
<select id="loc"><option value="id">Indonesia</option><option value="en">English</option></select>
<button id="run">Jalankan &#9654;</button>
<button id="stop">Stop &#9632;</button>
</div>
<pre id="out"></pre>
<script>
var $=function(id){return document.getElementById(id)};
var poll=null;
/* ponytail: keyword list is hand-copied from locales/{id,en}.json; if a builtin
   is added there, add it here too (highlight-only, won't break the language). */
var HL=/(\/\/[^\n]*)|("(?:[^"\\]|\\.)*")|(\b\d+(?:\.\d+)?\b)|\b(jika|lainnya|ulangi|selama|fungsi|kembali|benar|salah|dan|atau|bukan|buat|berhenti|if|else|repeat|while|function|return|true|false|and|or|not|make|stop|cetak|tanya|maju|mundur|belok_kanan|belok_kiri|warna|angkat_pena|turunkan_pena|nyalakan|matikan|baca_analog|baca|tunggu|print|ask|forward|backward|turn_right|turn_left|color|pen_up|pen_down|turn_on|turn_off|read_analog|read|wait)\b/g;
function esc(s){return s.replace(/[&<>]/g,function(c){return c=='&'?'&amp;':c=='<'?'&lt;':'&gt;';});}
function paint(){var s=$('src'),h=$('hl');
  h.innerHTML=esc(s.value).replace(HL,function(m,c,st,n){var t=c?'ct':st?'st':n?'nu':'kw';return '<span class='+t+'>'+m+'</span>';})+'\n';
  h.scrollTop=s.scrollTop;h.scrollLeft=s.scrollLeft;}
$('src').addEventListener('input',paint);
$('src').addEventListener('scroll',function(){var s=$('src'),h=$('hl');h.scrollTop=s.scrollTop;h.scrollLeft=s.scrollLeft;});
$('src').addEventListener('keydown',function(e){
  if(e.key!='Enter')return;
  e.preventDefault();
  var s=$('src'),v=s.value,p=s.selectionStart,ls=v.lastIndexOf('\n',p-1)+1;
  var line=v.slice(ls,p),ind=line.match(/^[ \t]*/)[0];
  if(line.replace(/\s+$/,'').slice(-1)=='{')ind+='  ';
  var ins='\n'+ind;
  s.value=v.slice(0,p)+ins+v.slice(s.selectionEnd);
  s.selectionStart=s.selectionEnd=p+ins.length;
  paint();
});
paint();
function stopPoll(){if(poll){clearInterval(poll);poll=null;}}
$('run').onclick=async function(){
  $('out').textContent='';
  var en=$('loc').value=='en';
  try{
    var rr=await fetch('/run?locale='+$('loc').value,{method:'POST',body:$('src').value});
    if(!rr.ok){$('out').textContent=(en?'Board: ':'Papan: ')+(await rr.text());return;}
  }catch(e){$('out').textContent=String(e);return;}
  stopPoll();
  poll=setInterval(async function(){
    try{
      var r=await fetch('/output');
      var t=await r.text();
      if(t){var o=$('out');o.textContent+=t;
        if(o.textContent.length>4000)o.textContent=o.textContent.slice(-4000);
        o.scrollTop=o.scrollHeight;}
      if(r.headers.get('X-Running')!='1'&&!t){
        stopPoll();
        if(!$('out').textContent)$('out').textContent=en?'(done)':'(selesai)';
      }
    }catch(e){stopPoll();$('out').textContent+='\n'+(en?'(board stopped answering)':'(papan berhenti menjawab)');}
  },300);
};
$('stop').onclick=function(){fetch('/stop',{method:'POST'});};
</script>
</body></html>
)AKSA";
