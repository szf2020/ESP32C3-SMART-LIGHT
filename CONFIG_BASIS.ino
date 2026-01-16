/*<table>
  <tr><td style='width:70px;'>switch name:<td><input class='inp6' name='swname' maxlength='21' title='no spaces, see help' value='{nm}'></input></tr>
  <tr><td>user passwd:<td><input  class='inp5' name='pw1' length='11' placeholder='max. 10 char' value='{pw1}' pattern='.{4,10}' title='between 4 en 10 characters'></input> 
  </td></tr>
  <tr><td>security:<td><input class='inp4' type='time' name='bev' value='{bev}' title='hh:mm' ></input></tr>
  <tr><td>duty adjust:<td><input class='inp2' type='number' name='lim' value='{lim}' min='50' max='100' title='50-100' ></input></tr>
  <tr><td>debounce:<td><input class='inp2' type='number' name='deb' value='{deb}' min='50' max='200' title='50-200' ></input></tr>
  
  <tr><td>serial debug:<td><input type='checkbox' style='width:30px; height:30px;' name='debug' #check></input></tr>

  </td></tr></table>
*/
const char BASISCONFIG[] PROGMEM = R"=====(
<body>
<div id='msect'><div id='bo'></div>

<div id="menu">
<a href="#" id="sub" onclick='submitFunction()'>save</a>
<a href="#" class='close' onclick='cl();'>&times;</a>
</div>
</div>
<div id='msect'>
<h2>ESP32 DIMMER SETTINGS</h2>
</div>

<div id='msect'>
  <div class='divstijl'>
  <center>
  <form id='formulier' method='get' action='submitform' oninput='showSubmit()'>
  
  <table>
  <tr><td style='width:120px;'>switch name:<td><input class='inp6' name='dvname' maxlength='21' title='no spaces, see help' value='{nm}'></input></tr>
  <tr><td>user passwd:<td><input  class='inp5' name='pw1' length='11' placeholder='max. 10 char' value='{pw1}' pattern='.{4,10}' title='between 4 en 10 characters'></input> 
  </td></tr>
  <tr><td>security:<td><input class='inp4' type='time' name='bev' value='{bev}' title='hh:mm' ></input></tr>
  <tr><td>duty adjust:<td><input class='inp2' type='number' name='lim' value='{lim}' min='50' max='100' title='50-100' ></input></tr>
  <tr><td>debounce:<td><input class='inp2' type='number' name='deb' value='{deb}' min='20' max='200' title='50-200' ></input></tr>
  
  <tr><td>serial debug:<td><input type='checkbox' style='width:30px; height:30px;' name='debug' #check></input></tr>

  </td></tr></table>
  
  </div><br>
</div>
</body></html>
)=====";

void zendPageBasis(AsyncWebServerRequest *request) {
  String(webPage)="";
    //DebugPrintln("we zijn nu op zendPageBasis");
    webPage = FPSTR(HTML_HEAD);
    webPage += FPSTR(BASISCONFIG);
    //Serial.println("zendpageBasis dvName = " + String(dvName)); 
    // replace data
    webPage.replace( "'{nm}'" , "'" + String(dvName) + "'") ;
    webPage.replace( "'{pw1}'" , "'" + String(userPwd) + "'") ;
    //webPage.replace( "'{del}'" , "'" + String(del) + "'") ;
//    if( timersEnabled ) { webPage.replace("#sjek", "checked");}
    webPage.replace("'{bev}'" , "'" + String(aso) + "'") ;
    webPage.replace("'{lim}'" , "'" + String(dutyAdjust) + "'") ;
    webPage.replace("'{deb}'" , "'" + String(deBounce) + "'") ;
    if (diagNose) { 
      webPage.replace("#check", "checked");
    }    
    request->send(200, "text/html", webPage);
    webPage=""; // free up
}
