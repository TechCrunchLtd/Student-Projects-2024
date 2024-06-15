
const wid = window.innerWidth;
const hgt = window.innerHeight;
const start_wid = wid/4 - 120;
const end_wid = wid/4 + 100;
const end_top = hgt/2;
const start_top = hgt/2 -270;


// Make the DIV element draggable:
dragElement(document.getElementById("probeRA"));
dragElement(document.getElementById("probeLA"));
dragElement(document.getElementById("probeLL"));

function dragElement(elmnt) {
  var pos1 = 0, pos2 = 0, pos3 = 0, pos4 = 0;
  if (document.getElementById(elmnt.id.substring(5))) {
    // if present, the header is where you move the DIV from:
    document.getElementById(elmnt.id.substring(5)).onmousedown = dragMouseDown;
  } else {
    // otherwise, move the DIV from anywhere inside the DIV:
    elmnt.onmousedown = dragMouseDown;
  }

  function dragMouseDown(e) {
    e = e || window.event;
    e.preventDefault();
    // get the mouse cursor position at startup:
    pos3 = e.clientX;
    pos4 = e.clientY;
    document.onmouseup = closeDragElement;
    // call a function whenever the cursor moves:
    document.onmousemove = elementDrag;
  }

  function elementDrag(e) {
    e = e || window.event;
    e.preventDefault();
    // calculate the new cursor position:
    pos1 = pos3 - e.clientX;
    pos2 = pos4 - e.clientY;
    pos3 = e.clientX;
    pos4 = e.clientY;
    // console.log(pos1, pos2, pos3, pos4);
    // set the element's new position:
    elmnt.style.top = (elmnt.offsetTop - pos2) + "px";
    elmnt.style.left = (elmnt.offsetLeft - pos1) + "px";
  }

  function closeDragElement() {
    // stop moving when mouse button is released:
    document.onmouseup = null;
    document.onmousemove = null;

    setProbePosition(0,0,0);
  }
}


function submitProbeData()
{
  const probeRAX = document.getElementById("probeRA").offsetLeft;
  const probeRAY = document.getElementById("probeRA").offsetTop;
  const probeLAX = document.getElementById("probeLA").offsetLeft;
  const probeLAY = document.getElementById("probeLA").offsetTop;
  const probeLLX = document.getElementById("probeLL").offsetLeft;
  const probeLLY = document.getElementById("probeLL").offsetTop;

  console.log("RA=", probeRAX, probeRAY);
  console.log("LA=", probeLAX, probeLAY);
  console.log("LL=", probeLLX, probeLLY);
  console.log("limits", start_wid, end_wid, start_top, end_top);

  if((probeRAX < start_wid) || (probeRAX > end_wid) || (probeRAY < start_top) || (probeRAY > end_top))
  {
    alert("1 out of range");
  }
  else if((probeLAX < start_wid) || (probeLAX > end_wid) || (probeLAY < start_top) || (probeLAY > end_top))
  {
    alert("2 out of range");
  }
  else if((probeLLX < start_wid) || (probeLLX > end_wid) || (probeLLY < start_top) || (probeLLY > end_top))
  {
    alert("3 out of range");
  }
  else if(((probeRAX <= probeLAX+10) && (probeRAX <= probeLLX+10)) || (Math.abs(probeLAY - probeLLY) < 10))
  {
    alert("Probes not positioned properly");
  }
  else
  {
    if(probeLAY < probeLLY) setProbePosition(1,2,3);
    else if(probeLLY < probeLAY) setProbePosition(1,3,2);
    
    const probePos = {
      RA: [probeRAX, probeRAY],
      LA: [probeLAX, probeLAY],
      LL: [probeLLX, probeLLY]
    };

    sendData("analyser", JSON.stringify(probePos));
  }
  
}

function revokeProbeData()
{
  sendData("halt", "now");
}

function setProbePosition(lead1, lead2, lead3)
{
  document.getElementById("leadRA").innerHTML = lead1;
  document.getElementById("leadLA").innerHTML = lead2;
  document.getElementById("leadLL").innerHTML = lead3;
}