
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
        $('#mobile').attr("value", config.user);
        $('#wifiname').attr("value", config.wifi);
    });

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
        
        $('#volt').html(status.volt);
        $('#curr').html(status.curr);
        $('#fault').html(status.fault);
        $('#line').html(status.line);

    });
}

function updateSettings()
{
    const Wifi_name = document.getElementById('wifiname').value;
    const Wifi_key = document.getElementById('wifipassword').value;
    const usernumber = document.getElementById('mobile').value;

    $.post("/setconfig", {
            Wifi:Wifi_name,
            User: usernumber,
            Password: Wifi_key
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
