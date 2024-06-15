
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
        $('#defaultOpen').html(config.device.toUpperCase());
        $('#device_id').attr("value", config.device);
        $('#wifiname').attr("value", config.wifi);

        if(config.update == 1){
            $("#updates option[value = on]").prop('selected', true);
        }
        else {
            $("#updates option[value = off]").prop('selected', true);
        }

        $('#interval').attr("value", config.interval);

    });

    document.getElementById("loginpage").style.display = "block";

    const refreshApp = setInterval(updatePage, 1000);
});

function updatePage()
{
    if(document.getElementById("updates").value === "off"){
        document.getElementById("interval_input").style.display = "none";
    }
    else{
        document.getElementById("interval_input").style.display = "block";
    }
    
    $.get("/getstatus", function(data){
        console.log(data);
        const status = JSON.parse(data);

        if(status.allowed == 1){
            document.getElementById("loginpage").style.display = "none";
        }
        else {
            document.getElementById("loginpage").style.display = "block";
        }
        
        const curr_dev = status.dev.toUpperCase();
        if(document.getElementById("defaultOpen").innerHTML != curr_dev){
            $('#defaultOpen').html(curr_dev);
        }
        $('#temp').html(status.temp);
        $('#pulse').html(status.pulse);
        $('#spo2').html(status.spo2);

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

function updateSettings()
{
    const Update_state = document.getElementById('updates').value;
    const Wifi_name = document.getElementById('wifiname').value;
    const Update_iterval = document.getElementById('interval').value;
    const Wifi_key = document.getElementById('wifipassword').value;
    const Device_id = document.getElementById('device_id').value;

    $.post("/setconfig", {
            Name:Wifi_name,
            Device: Device_id,
            Password: Wifi_key,
            Update: Update_state,
            Interval: Update_iterval
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


function showChart()
{
    alert("Graph not supported!");
}
