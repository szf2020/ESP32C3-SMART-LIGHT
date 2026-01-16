const char MENUPAGE[] PROGMEM = R"=====(
<body>
<style>
<style>
#menu2 a:link, #menu2 a:visited {
  width: 98px;
}
</style>
<script>
function close() {
window.location.href='/';
}
</script>
<div id='msect'>
<div id="menu">
<a href='/ABOUT'>system info</a>
<a href="/" class='close' onclick='close();'>&times;</a>
</div>

<br><br>
<h2>ESP32 DIMMER MENU</h2></div><br><br>
<div class='divstijl'><center><br>
<div id="menu2">

<table><tr>
<td style="width:100px"><a href='/TIMERCONFIG'>timers</a></td><td style="width:60px"></td><td style="width:100px">
<a href='/LOGPAGE'>journal</a></<td></tr>

<tr><td><a href='/BASISCONFIG'>settings</a></td><td></td>
<td><a href='/FWUPDATE'>fw update</a></td></tr>

<tr><td><a href='/GEOCONFIG'>time config</a></td><td></td>
<td><a onclick="return confirm('are you sure?')" href='/REBOOT'>reboot</a></td></tr>

<tr><td><a href='/MQTT'>mosquitto</a></td><td></td>
<td><a onclick="return confirm('reset wifi, are you sure?')" href='/STARTAP?'>reset wifi</a></td><tr>

</table>
</div></div></div>
</body></html>
  )=====";
