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
textarea{flex:1;font:16px/1.5 ui-monospace,monospace;padding:8px;border:2px solid #888;border-radius:8px}
pre{background:#f0f0f0;border-radius:8px;padding:8px;min-height:4em;white-space:pre-wrap;margin:0;overflow:auto}
.row{display:flex;gap:8px}
button{flex:1;font-size:18px;padding:10px;border:0;border-radius:8px;background:#2563eb;color:#fff}
#stop{background:#dc2626}
select{font-size:16px;padding:6px;border-radius:8px}
</style></head><body>
<textarea id="src" spellcheck="false" autocapitalize="off" autocomplete="off" autocorrect="off">selama (benar) {
  buat data = baca_analog(0)
  tulis(data)
  jika (data > 700) {
    nyalakan(15)
  } lainnya {
    matikan(15)
  }
  tunggu(1000)
}</textarea>
<div class="row">
<select id="loc"><option value="id">Indonesia</option><option value="en">English</option></select>
<button id="run">Jalankan &#9654;</button>
<button id="stop">Stop &#9632;</button>
</div>
<pre id="out"></pre>
<script>
var $=function(id){return document.getElementById(id)};
var poll=null;
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
