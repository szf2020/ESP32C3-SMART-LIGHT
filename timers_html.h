
const char TIMERCONFIG_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<meta charset='utf-8'>
<title>ESP32-C3-DIMMER</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<style>
.nav { background: #eee; padding: 10px; }
.nav a { margin-right: 10px; cursor: pointer; color: blue; text-decoration: none; }
iframe { width: 100%; height: 400px; border: 1px solid #ccc; margin-top: 10px; }
.container { max-width: 600px; margin: auto; background: white; padding: 20px; border-radius: 8px; box-shadow: 0 4px 6px rgba(0,0,0,0.1); }
</style>
</head>
<body>
<div class="container">
    <div class="nav">
        <a onclick="loadTimer(0)">tmr 0</a>
        <a onclick="loadTimer(1)">tmr 1</a>
        <a onclick="loadTimer(2)">tmr 2</a>
        <a onclick="loadTimer(3)">tmr 3</a>
 <span style="font-size:40px; font-color:red;"><a href="/" class='close' style="color: red; text-decoration: none; float:right">&times;</a></span>   
</div>

    <!-- Hierin wordt de content geladen -->
    <iframe id="contentFrame" src="/timer?welke=%welketimer%"></iframe>

    <script>
        function loadTimer(id) {
            document.getElementById('contentFrame').src = "/timer?welke=" + id;
        }
    </script>
</body>
</html>
 )=====";
                                                                        
 const char TIMER_GENERAL[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
<style>
tr {height: 40px;}
.btn { background: #28a745; color: white; padding: 12px; border: none; width: 100%; margin-top: 25px; border-radius: 4px; cursor: pointer; font-size: 1em; font-weight: bold; }
body {font-family:arial; font-size: 14<td>px;}
tr {height: 40px;}
table {width:90%;}
</style></head><body>
<center><H1 style='color:#e207fa;'>TIMER {nr} </h1>
<form id='formulier' method='get' action='/submitTimers' target='_parent'>

<center><table>
<tr><td style='width:60px;'>active?<td style='width:80px'><input type='checkbox' name='ta' tActive>
<tr><td>level<td><input class ='inp3' type='number' name='lev' value="{lev}" min="10" max="100">


<tr>
<tr><td>on :
<td><input class='inp4' type='time' name='inw' value='{onX}' title='hh:mm'>
<td><select name='zonattaan' class='sb1'>
<option value='0' zonattaan_0>absolute</option>
<option value='1' zonattaan_1>before sunrise</option>
<option value='2' zonattaan_2>after sunrise</option>
<option value='3' zonattaan_3>before sunset</option>
<option value='4' zonattaan_4>after sunset</option>

</select></td></tr>
<tr><td>off :<td><input class='inp4' name='uitw' type='time' value='{ofX}' title='hh:mm' >
<td><select name='zonattuit' class='sb1'>
<option value='0' zonattuit_0>absolute</option>
<option value='1' zonattuit_1>before sunrise</option>
<option value='2' zonattuit_2>after sunrise</option>
<option value='3' zonattuit_3>before sunset</option>
<option value='4' zonattuit_4>after sunset</option>

</select></td></tr></table>
<br><table>
<tr><thead>
<th> su <input type='checkbox' name='su' selzo></th>
<th> mo <input type='checkbox' name='mo' selma></th>
<th> tu <input type='checkbox' name='tu' seldi></th>
<th> we <input type='checkbox' name='we' selwo></th>
<th> th <input type='checkbox' name='th' seldo></th>
<th> fr <input type='checkbox' name='fr' selvr></th>
<th> sa <input type='checkbox' name='sa' selza></th>
</tr></thead></table>


<button type="submit" class="btn">SAVE TIMER </button></td></tr>

</center><br>
 )=====";