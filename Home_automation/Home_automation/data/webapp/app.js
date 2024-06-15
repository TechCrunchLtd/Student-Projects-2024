
let alarm_count = 0;

$(document).ready( function(){
    
    window.onscroll = function() 
    {
        let header = document.getElementById("myHeader");
        let footer = document.getElementById("myFooter");
        let sticky = header.offsetTop;

        if (window.scrollY > sticky) {
            header.classList.add("sticky");
            header.style.fontSize = "8px";
            footer.style.fontSize = "15px";
        } 
        else {
            header.classList.remove("sticky");
            header.style.fontSize = "15px";
            footer.style.fontSize = "10px";
        }
    }
    
    document.getElementById("defaultOpen").click();

    $.get("/getconfig", function(data){
        console.log(data);
        let config = JSON.parse(data);

        $('#whatsapp').attr("value", config.number);
        $('#institute').attr("value", config.institute);

        if(config.alarm == 1){
            $("#alarmstate option[value = on]").prop('selected', true);
        }
        else {
            $("#alarmstate option[value = off]").prop('selected', true);
        }

        $('#duration').attr("value", config.duration);
    });

    // switch (new Date().getDay()) {
    //     case 0:
    //         pickDay("Sunday");
    //         break;
    //     case 1:
    //         pickDay("Monday");
    //         break;
    //     case 2:
    //         pickDay("Tuesday");
    //         break;
    //     case 3:
    //         pickDay("Wednesday");
    //         break;
    //     case 4:
    //         pickDay("Thursday");
    //         break;
    //     case 5:
    //         pickDay("Friday");
    //         break;
    //     case 6:
    //         pickDay("Saturday");
    //         break;
    //     default:
    //         pickDay("All days");
    // }

    const refreshApp = setInterval(updatePage, 1000);
    document.getElementById("loginpage").style.display = "block";
});

function updatePage()
{
    // let datestring = new Date()
    // $('#currentdatetime').html(datestring.toString().substring(0,24));

    $.get("/getstatus", function(data){
        console.log(data);
        const status = JSON.parse(data);
        // $('#lightdatetime').html(status.time);
        // $('#lighttime').html(status.time.substring(status.time.indexOf(':') - 2));
        // $('#nextalarm').html(status.alarm);

        if(status.activated == 1){
            $('#switchlight').html('Turn Off Light');
            $('#lightimage').attr("src", "light_on.jpg");
        }
        else{
            $('#switchlight').html('Light On Light');
            $('#lightimage').attr("src", "light_off.jpg");
        }

        if(status.allowed == 1){
            document.getElementById("loginpage").style.display = "none";
        }
        else {
            document.getElementById("loginpage").style.display = "block";
        }
    });
}

function handleLogin()
{
    let user = document.forms["logmein"]["username"].value;
    let pass = document.forms["logmein"]["userkey"].value;

    $.post("/login", {Username:user, Password:pass},
        function(data)
        {
            console.log(data);
            if(data != "true") { document.getElementById("loginmsg").innerHTML = "Login Failed"; }
        }
    );
    
    return false;
}

function switchLight()
{
    let light = "LIGHT";
    // if($("#switchlight").html() == "Ring Light"){ ring = 1; }
    // else { ring = 0; }

    $.get("/switchlight", {Switch:light}, function(data){
        console.log(data);
        if(data != light) { alert("Command Failed"); }
    });
}

function pickDay(thisDay)
{
    let thisday = thisDay.toUpperCase();
    $("#selectday").html(thisday + '<i class="arrow"></i>');

    if(thisday != "ALL DAYS")
    {
        while(alarm_count > 0){
            addAlarm('sub');
        }

        thisday = thisday.substring(0,3).toLowerCase();

        $.get("/getalarms", {Day:thisday}, function(data){
            console.log(data);
            let recv_data = JSON.parse(data);
            if(recv_data.day == thisday){
                alarm_count = 0;
                for(let x in recv_data.alarms){
                    addAlarm('add');
                    document.getElementById('pd' + (alarm_count-1)).value = recv_data.alarms[x];
                }
            }
        });
    }
}

function addAlarm(more)
{
    if(more == "add"){
        let alarm_input = '<input type="time" id="pd' + alarm_count + '">';
        document.getElementById("alarmfields").insertAdjacentHTML("beforeend", alarm_input);
        alarm_count++;
    }
    else if(more == "sub"){
        if(alarm_count > 0){
            alarm_count--;
            let alarm_id = 'pd' + alarm_count;
            document.getElementById(alarm_id).remove();
        }
        else{alert("No more alarms!");}
    }
}

function updateAlarms()
{
    let day = $("#selectday").html().substring(0,3).toLowerCase();
    let alarms = "";
    let alarm_index = 0;

    for(let x=0; x<alarm_count; x++){
        let new_alarm = document.getElementById('pd'+x).value;
        if(new_alarm.length == 5) { alarms += new_alarm + ','; }
    }
    console.log(alarms);

    $.post("/setalarms", {Day:day, Alarms: alarms}, function(data){
        if(data == "true"){ alert("periods update successful"); }
        else { alert("failed to update periods"); }
    });

    return false;
}

function updateSettings()
{
    const Alarmstate = document.getElementById('alarmstate').value;
    const Institute = document.getElementById('institute').value;
    const Duration = document.getElementById('duration').value;
    const Userkey = document.getElementById('newuserkey').value;
    const Whatsapp = document.getElementById('whatsapp').value;
    // const Newdate = new Date().toString().substring(4,24);

    $.post("/setconfig", {
            Name:Institute,
            Number: Whatsapp,
            Password: Userkey,
            Alarms: Alarmstate,
            Ringduration: Duration
        }, 
        function(data){
            if(data == "true"){ alert("settings update successful"); }
            else { alert("failed to update settings"); }
        }
    );

    return false;
}

function openPage(pageName,elmnt) 
{
    let i, tabcontent, tablinks;
    tabcontent = document.getElementsByClassName("tabcontent");
    for (i = 0; i < tabcontent.length; i++) {
        tabcontent[i].style.display = "none";
    }
    tablinks = document.getElementsByClassName("tablink");
    for (i = 0; i < tablinks.length; i++) {
        tablinks[i].style.color = "black"
        tablinks[i].style.backgroundColor = "#35f8b1";
    }
    document.getElementById(pageName).style.display = "block";
    elmnt.style.backgroundColor = "#555";
    elmnt.style.color = "white";
}
