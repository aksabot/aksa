/* Editor page served at "/": textarea, locale picker, Run button, output box.
   Self-contained — the board is offline, so no external assets. */
static const char AKSA_INDEX_HTML[] PROGMEM = R"AKSA(<!doctype html>
<html><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1">
<title>Aksa</title>
<style>
body{font-family:system-ui,sans-serif;margin:12px;display:flex;flex-direction:column;gap:8px;height:94vh}
textarea{flex:1;font:16px/1.5 ui-monospace,monospace;padding:8px;border:2px solid #888;border-radius:8px}
pre{background:#f0f0f0;border-radius:8px;padding:8px;min-height:4em;white-space:pre-wrap;margin:0}
.row{display:flex;gap:8px}
button{flex:1;font-size:18px;padding:10px;border:0;border-radius:8px;background:#2563eb;color:#fff}
select{font-size:16px;padding:6px;border-radius:8px}
</style></head><body>
<textarea id="src" spellcheck="false" autocapitalize="off" autocomplete="off" autocorrect="off">nyalakan(8)
tunggu(500)
matikan(8)</textarea>
<div class="row">
<select id="loc"><option value="id">Indonesia</option><option value="en">English</option></select>
<button id="run">Jalankan &#9654;</button>
</div>
<pre id="out"></pre>
<script>
var $=function(id){return document.getElementById(id)};
$('run').onclick=async function(){
  $('out').textContent='...';
  try{
    var r=await fetch('/run?locale='+$('loc').value,{method:'POST',body:$('src').value});
    $('out').textContent=await r.text()||'(selesai / done)';
  }catch(e){$('out').textContent=String(e)}
};
</script>
</body></html>
)AKSA";
