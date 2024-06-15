const express = require('express');
const app = express();
const multer = require('multer');
const path = require('path');
const fs = require('fs');
const cors = require('cors');
const dotenv = require('dotenv');
dotenv.config();

const dbService = require('./dbService')

let allowedAdmins = [];

const clearAdmins = setInterval( () => {
    // console.log(allowedAdmins);
    const oldAdmins = allowedAdmins;
    allowedAdmins = [];
    oldAdmins.forEach(admin =>{
        if(admin.delay < 60000){
            allowedAdmins.push(admin);
        }
    });

    allowedAdmins.forEach(admin => {
        admin.delay += 10000;
    });
},10000);

function deleteAdminToken(token){
    // console.log(allowedAdmins);
    const oldAdmins = allowedAdmins;
    allowedAdmins = [];
    oldAdmins.forEach(admin =>{
        if(admin.token != token){
            allowedAdmins.push(admin);
        }
    });

    allowedAdmins.forEach(admin => {
        admin.delay += 10000;
    });
}

// Set up Multer for handling file uploads
const storage = multer.diskStorage({
    destination: function (req, file, cb) {
      cb(null, 'public/images/');
    },
    filename: function (req, file, cb) {
      // Use the filename provided by the client
      console.log(req);
      cb(null, `${req.body.filename}.png`);
    }
  });

const upload = multer({ storage: storage });

//serve app
app.use(express.static('public'));

// app.use(cors());
app.use(express.json());
app.use(express.urlencoded({ extended : false}));

//login temp
app.get('/login', (req, res) => {
    // console.log(req.query);
    const user = req.query.username;
    const pass = req.query.password;
    const user_token = req.query.token;
    // console.log(user, pass, user_token);

    const db = dbService.getDbServiceInstance();
    const result = db.getUsers(user);

    result
    .then(data => {
        // console.log(data)
        let got_user = false;
        if((data[0].email === user) && (data[0].password === pass)){
            let admin_level = false;
            if(data[0].superAdmin == true){
                admin_level = true;
            }
            allowedAdmins.push({token:user_token, superAdmin: admin_level, delay: 0});
            if(admin_level == true){
                res. json({allowed: 'admin'});
            }
            else{
                res. json({allowed: 'yes'});
            }
        }
        else{
            res. json({allowed: 'No'});
        }
    })
    .catch(err => console.log(err))
});
//logout admin
app.get('/logout', (req, res) => {
    // console.log(req.query);
    const user_token = req.query.token;
    // console.log(user_token);

    deleteAdminToken(user_token);

    res. json({allowed: 'yes'});
});

//search all possibilities
app.get('/search', (req, res) => {
    // console.log(req.query);
    const search_key = req.query.keyword;
    const db = dbService.getDbServiceInstance();
    const result = db.getAllDetainees(search_key);

    
    result
    .then(data => {
        
        res.json({data : data});
        
    })
    .catch(err => console.log(err))
});


//search for specific
app.get('/searched', (req, res) => {
    // console.log(req.query);
    const search_key = req.query.keyword;
    const db = dbService.getDbServiceInstance();
    const result = db.getSpecificDetainee(search_key);

    result
    .then(data => {
        // console.log(data)
        res.json(data[0]);
    })
    .catch(err => console.log(err))
});

app.get('/update', (req, res) => {
    const userkey = req.query.token;
    // console.log(userkey);
    let foundUser = false;

    if(userkey.length > 10){
        allowedAdmins.forEach(admin => {
            if(admin.token === userkey) {
                foundUser = true;
                admin.delay = 0;
                res. json({allowed: 'yes'});
            }
        });

        if(foundUser == false){
            res. json({allowed: 'No'});
        }
    }
    else {
        res. json({allowed: 'No'});
    }
});

//query detainees
app.get('/querybydate', (req, res) => {
    // console.log(req.query);
    const startD = req.query.startdate;
    const stopD = req.query.stopdate;

    const db = dbService.getDbServiceInstance();
    const result = db.queryUsersByDate(startD, stopD);

    result
    .then(data => {
        // console.log(data)
        res.json({data : data})
    })
    .catch(err => console.log(err))
});

//get all centers
app.get('/centers', (req, res) => {
    // console.log(req.query);
    const db = dbService.getDbServiceInstance();
    const result = db.getAllCenters();

    result
    .then(data => {
        res.json({data : data});       
    })
    .catch(err => console.log(err));
});

app.post('/addcenter', (req, res) => {
    // console.log(req.body)
    const db = dbService.getDbServiceInstance();
    const result = db.updateCenter(false, req.body);
    result.then(response => {
        console.log(response);
        res.json({saved: true});
    })
    .catch(err => console.log(err));
});


app.post('/updatedetainee', (req, res) => {
    // console.log(req.body)
    const db = dbService.getDbServiceInstance();
    const result = db.updateDetainee(true, req.body);
    result.then(response => {
        // console.log(response);
        res.json({saved: true});
    })
    .catch(err => console.log(err));
});

//upload avatar
app.post('/avatars', upload.single('file'), (req, res) => {
    // console.log(req.file.path);
    fs.renameSync(req.file.path, req.file.path.replace('undefined', req.body.filename));
    res.json({saved: true});
});

//add new detainee
app.post('/newdetainee', (req, res) => {
    const db = dbService.getDbServiceInstance();
    const result = db.updateDetainee(false, req.body);
    result.then(response => {
        // console.log(response);
        res.json({saved: true});
    })
    .catch(err => console.log(err));
});


app.listen(process.env.PORT, () => {
    console.log("server running");
})

/* ----------------------------------- EOF ---------------------------------- */