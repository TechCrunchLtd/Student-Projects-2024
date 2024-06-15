const userToken = getUniqueToken(
	16,
	"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
);
let userAllowed = false;
let superAdmin = false;

let centerArr = [];

document.addEventListener("DOMContentLoaded", function () {
	window.onscroll = function () {
		let header = document.getElementById("myHeader");
		let footer = document.getElementById("myFooter");
		let sticky = header.offsetTop;

		if (window.scrollY > sticky) {
			header.classList.add("sticky");
			header.style.fontSize = "8px";
			footer.style.fontSize = "15px";
		} else {
			header.classList.remove("sticky");
			header.style.fontSize = "15px";
			footer.style.fontSize = "10px";
		}
	};

	document.getElementById("defaultOpen").click();
	// document.getElementById("adminPanel").click();

	// the login page should be displayed at the begining
	document.getElementById("loginpage").style.display = "block";
	document.getElementById("reportpage").style.display = "none";
	document.getElementById("logoutpoint").style.display = "none";

	const refreshApp = setInterval(updatePage, 5000);
});

function getUniqueToken(length, chars) {
	var result = "";
	for (var i = length; i > 0; --i)
		result += chars[Math.floor(Math.random() * chars.length)];
	return result;
}

function updatePage() {
	fetch(`/update?token=${userToken}`)
		.then((response) => response.json())
		.then((data) => {
			console.log(data);
			if (data.allowed === "yes") {
				document.getElementById("loginpage").style.display = "none";
				document.getElementById("logoutpoint").style.display = "block";
				userAllowed = true;
			} else {
				document.getElementById("loginpage").style.display = "block";
				document.getElementById("logoutpoint").style.display = "none";
				userAllowed = false;
				superAdmin = false;
			}
		});
}

function handleLogin() {
	let user = document.forms["logmein"]["username"].value;
	let pass = document.forms["logmein"]["userkey"].value;
	// alert(user);

	fetch(`/login?username=${user}&password=${pass}&token=${userToken}`)
		.then((response) => response.json())
		.then((data) => {
			if (data.allowed === "admin") {
				document.getElementById("loginpage").style.display = "none";
				document.getElementById("logoutpoint").style.display = "block";
				userAllowed = true;
				superAdmin = true;
				loadCenters();
			} else if (data.allowed === "yes") {
				document.getElementById("loginpage").style.display = "none";
				document.getElementById("logoutpoint").style.display = "block";
				userAllowed = true;
				superAdmin = false;
				loadCenters();
			} else {
				document.getElementById("loginpage").style.display = "block";
				document.getElementById("logoutpoint").style.display = "none";
				document.getElementById("loginmsg").innerHTML =
					"Incorrect username or password";
			}
		});

	document.forms["logmein"]["userkey"].value = "";

	return false;
}

function handleLogout() {

	fetch(`/logout?token=${userToken}`)
		.then((response) => response.json())
		.then((data) => {
			console.log(data)
			document.getElementById("logoutpoint").style.display = "none";
			document.getElementById("loginpage").style.display = "block";
			document.getElementById("loginmsg").innerHTML =
					"You have logged out";
			userAllowed = false;
			superAdmin = false;
		});

	document.forms["logmein"]["userkey"].value = "";
}

function handleReports(){
	document.getElementById("reportpage").style.display = "block";

	const start_date = document.getElementById("start-date").value;
	const stop_date = document.getElementById("stop-date").value;

	let tableHtml = "<tr><th>Date of Arrest</th><th>Full Names</th><th>Area of Arrest</th><th>Detained at(P/S)</th></tr>";
	
	if(start_date.length < 5 || stop_date.length < 5){
		tableHtml += `<tr><td>-</td><td>-</td><td>-</td><td>-</td></tr>`;
		document.getElementById("filled-report").innerHTML = tableHtml;
	}
	else {		
		const header = `<h4>Detainees from ${start_date} to ${stop_date}</h4>`;
		const footer = `<p>printed ${new Date()}</p>`;
		document.getElementById('table-header').innerHTML = header;
		document.getElementById('table-footer').innerHTML = footer;

		const date1 = getDatePlusDay(start_date);
		const date2 = getDatePlusDay(stop_date);
		
		fetch(`/querybydate?startdate=${date1}&stopdate=${date2}`)
			.then((response) => response.json())
			.then((reports) => {
				// console.log(reports);
				reports.data.forEach((person) => {
					const dt = new Date(person.arrestDate);
					tableHtml += `<tr><td>${dt.toLocaleString()}</td><td>${person.fullNames}</td><td>${person.arrestArea}</td><td>${person.centerName}</td></tr>`;
				});
				console.log(tableHtml);
				document.getElementById("filled-report").innerHTML = tableHtml;
			});
	}


}

function printReport(){
	let doc = new jspdf.jsPDF('l', 'pt', 'a4');

	doc.html(document.querySelector('#reported-table'), {
		callback: function (doc) {
			doc.save('detainee.pdf');
		},
		margin: [30, 30, 30, 30],
		x: 22,
		y: 32,
	});
}

function handleSearch() {
	let detainee = document.forms["search-form"]["search"].value;
	document.getElementById("detainee-card").innerHTML = "";

	fetch(`/search?keyword=${detainee}`)
		.then((response) => response.json())
		.then((data) => {
			console.log(data);
			const resultElement = document.getElementById("detainee-results");
			let resultHtml = "";

			data.data.forEach((person) => {
				resultHtml += `<div class="chip" onclick="loadCard('${person.NIN}')">
                            <img src="/images/${person.NIN}.png" alt="Image" onerror="this.src='images/img_avatar.png'" width="96" height="96">
                            <span>${person.fullNames}</span>
                            </div>`;
			});

			if (resultHtml.length > 10) {
				resultElement.innerHTML = resultHtml;
			} else {
				resultElement.innerHTML = "Not Found";
			}
		});

	return false;
}

function loadCard(detaineeNIN) {
	fetch(`/searched?keyword=${detaineeNIN}`)
		.then((response) => response.json())
		.then((person) => {
			console.log(person);
			const resultElement = document.getElementById("detainee-card");
			let resultHtml = `<img src="/images/${person.NIN}.png" alt="${person.fullNames}" onerror="this.src='images/img_avatar.png'" style="width:100%">
                        <h1>${person.fullNames}</h1>
                        <p>${person.NIN}</p>
                        <p>${person.age} years old</p>
                        <p>${person.fullNames} was arrested from ${person.arrestArea} on ${person.arrestDate.substring(0,19)} with charges of ${person.charges} and is currently detained at 
                        <a href="${person.mappin}" target="_blank">${person.centerName}</a>, 
                        located in ${person.location}
                        </p>`;
			if (userAllowed == true) {
				resultHtml += `<p><button class="pointer" onclick="editData('${person.NIN}')">Edit</button></p>`;
			}

			resultElement.innerHTML = resultHtml;
		});

	document.getElementById("detainee-results").innerHTML = "";
}

function editData(detainee_NIN) {
	// alert("edit form");
	// populate detainee form
	fetch(`/searched?keyword=${detainee_NIN}`)
		.then((response) => response.json())
		.then((person) => {
			document.getElementById("fullname").value = person.fullNames;
			document.getElementById("national_id").value = person.NIN;
			document.getElementById("age").value = person.age;
			document.getElementById("charged").value = person.charges;
			document.getElementById("center_id").value = person.centerId;
			document.getElementById("arrest_area").value = person.arrestArea;
			document.getElementById("arrest_date").value = person.arrestDate.substring(0,19);
			document.getElementById("edit").value = "2";
			document.getElementById(
				"passport"
			).src = `images/${person.NIN}.png`;

			document.getElementById("national_id").readOnly = true;
			// document.getElementById('national_id').setAttribute('readonly', true);
		});
	document.getElementById("adminPanel").click();
}

function updateSettings() {
	const name = document.getElementById("fullname").value.trim();
	const id_nb = document.getElementById("national_id").value.trim();
	const age_yrs = document.getElementById("age").value;
	// const arrested = getDatePlusDay(document.getElementById("arrest_date").value);
	const arrested = document.getElementById("arrest_date").value;
	const arrest_point = document.getElementById("arrest_area").value;
	const center = document.getElementById("center_id").value;
	const charges = document.getElementById("charged").value;
	const passport = document.getElementById("pp_file");

	alert(arrested);

	let filedata = new FormData();
	filedata.append("file", passport.files[0]);
	filedata.append("filename", `${id_nb}`);

	fetch("/avatars", {
		method: "POST",
		body: filedata,
	})
		.then((response) => response.json())
		.then((data) => {
			console.log(data);
		});

	let endpoint = "";
	if (document.getElementById("edit").value == "2") {
		endpoint = "updatedetainee";
	} else {
		endpoint = "newdetainee";
	}

	fetch(`/${endpoint}`, {
		headers: {
			"Content-type": "application/json",
		},
		method: "POST",
		body: JSON.stringify({
			name: name,
			nin: id_nb,
			age: age_yrs,
			center: center,
			charges: charges,
			area: arrest_point,
			arrested: arrested,
		}),
	})
		.then((response) => response.json())
		.then((data) => {
			console.log(data);
			if (data.saved == true) {
				alert("saved successfully");
			}
			document.getElementById("reset-btn").click();
		});

	return false;
}

function loadCenters() {
	let centerHtml = "";
	centerArr = [];

	fetch("/centers")
		.then((response) => response.json())
		.then((centers) => {
			// console.log(centers);
			centers.data.forEach((center) => {
				// console.log(center);
				centerHtml += `<option value="${center.centerId}">${center.centerName},${center.location}</option>`;
				centerArr.push(center.centerName+center.location);
			});
			// console.log(centerHtml);
			if (centerHtml.length > 10) {
				if(superAdmin == true){
					centerHtml +=
						'<option value="999"><button class="pointer">New center...</button></option>';
				}
				document.getElementById("center_id").innerHTML = centerHtml;
			}
		});
}

function openCenterForm() {
	if ((document.getElementById("center_id").value == "999") && (superAdmin ==true)) {
		//add center
		document.getElementById("createcenterPage").style.display = "block";
	}
}

function closeCenterForm() {
	document.getElementById("createcenterPage").style.display = "none";
	document.getElementById("adminPanel").click();
}

function newCenter() {
	const name = document.getElementById("centername").value;
	const area = document.getElementById("location").value;
	const map = document.getElementById("mappin").value;

	let centerExists = false;
	const newcenter = name+area;
	//check if center already exists
	centerArr.forEach((center) => {
		if(newcenter == center)
		{
			alert(`${name} already exists!`);
			centerExists = true;
		}
	});

	if(centerExists == false)
	{
		fetch(`/addcenter?token=${userToken}`, {
			headers: {
				"Content-type": "application/json",
			},
			method: "POST",
			body: JSON.stringify({
				centername: name,
				location: area,
				mappin: map,
			}),
		})
		.then((response) => response.json())
		.then((data) => {
			console.log(data);
			if (data.saved == true) {
				alert("saved successfully");
				loadCenters();
			}
			document.getElementById("createcenterPage").style.display = "none";
			document.getElementById("adminPanel").click();
		});
}
	return false;
}

function previewPassport() {
	const [file] = document.getElementById("pp_file").files;
	if (file) {
		document.getElementById("passport").src = URL.createObjectURL(file);
		// document.getElementById('passport').setAttribute('src', event.target.result);
	}
}

function clearPassport() {
	document.getElementById("passport").src = "images/img_avatar.png";
	document.getElementById("national_id").removeAttribute("readonly");
}

function openPage(pageName, elmnt) {
	let i, tabcontent, tablinks;
	tabcontent = document.getElementsByClassName("tabcontent");
	for (i = 0; i < tabcontent.length; i++) {
		tabcontent[i].style.display = "none";
	}
	tablinks = document.getElementsByClassName("tablink");
	for (i = 0; i < tablinks.length; i++) {
		tablinks[i].style.color = "black";
		tablinks[i].style.backgroundColor = "#35f8b1";
	}
	document.getElementById(pageName).style.display = "block";
	elmnt.style.backgroundColor = "#1a055d";
	elmnt.style.color = "white";
}

function getDatePlusDay(today)
{
	const d = new Date(today);
	d.setTime(d.getTime()+ (24*60*60*1000));
	return d.getFullYear() + "-" + (d.getMonth()+1) + "-" + d.getDate();
}