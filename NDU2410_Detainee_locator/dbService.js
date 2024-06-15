const mysql = require('mysql');
const dotenv = require('dotenv');
const { resolve } = require('path');
dotenv.config();

let instance = null;

const connection = mysql.createConnection({
    host: process.env.HOST,
    user: process.env.USERNAME,
    password: process.env.PASSWORD,
    database: process.env.DATABASE,
    port: process.env.DB_PORT
});

connection.connect((err) => {
    if(err) {
        console.log(err.message);
    }
    console.log('db ' + connection.state);
})

class DbService {
    static getDbServiceInstance(){
        return instance ? instance : new DbService();
    }

    async getAllDetainees(kew_word) {
        try {
            const response = await new Promise((resolve, reject) => {
                const query = `SELECT NIN, fullNames FROM detainees WHERE NIN = '${kew_word}' OR fullNames LIKE '%${kew_word}%' `;
                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }

    async getSpecificDetainee(nin) {
        try {
            const response = await new Promise((resolve, reject) => {
                const query = `SELECT * FROM detainees INNER JOIN detaineecenters ON detainees.center=detaineecenters.centerId WHERE NIN = '${nin}'`;
                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }

    async getUsers(user_id) {
        try {
            const response = await new Promise((resolve, reject) => {
                const query = `SELECT * FROM adminusers WHERE email = '${user_id}'`;
                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }
    
    async queryUsersByDate(startdt, stopdt) {
        // console.log(startdt, stopdt);
        try {
            const response = await new Promise((resolve, reject) => {
                const query = `SELECT * FROM detainees 
                                INNER JOIN detaineecenters ON detainees.center=detaineecenters.centerId 
                                WHERE arrestDate BETWEEN DATE('${startdt}') AND DATE('${stopdt}')
                                ORDER BY arrestDate ASC`;
                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }
    
    async getAllCenters() {
        try {
            const response = await new Promise((resolve, reject) => {
                const query = 'SELECT * FROM detaineecenters ';
                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }

    async updateCenter(update, data) {
        try {
            const response = await new Promise((resolve, reject) => {
                let query = "";
                if(update == true) {
                    query = `UPDATE detaineecenters 
                            SET centerName = '${data.centername}', location = '${data.location}', mappin='${data.mappin}'  
                            WHERE (centerId = '${data.id}')`;
                }
                else {
                    query = `INSERT INTO detaineecenters (centerName, location, mappin) 
                            VALUES ('${data.centername}', '${data.location}', '${data.mappin}')`;
                }

                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }

    async updateDetainee(update, data) {
        try {
            const response = await new Promise((resolve, reject) => {
                let query = "";
                if(update == true) {
                    query = `UPDATE detainees 
                            SET fullNames = '${data.name}', age = '${data.age}', center='${data.center}', charges = '${data.charges}', arrestDate = DATETIME'${data.arrested}, arrestArea '${data.area}' 
                            WHERE (NIN = '${data.nin}')`;
                }
                else {
                    query = `INSERT INTO detainees (NIN, fullNames, age, center, charges, arrestDate, arrestArea) 
                            VALUES ('${data.nin}', '${data.name}', '${data.age}', '${data.center}', '${data.charges}', '${data.arrested}', '${data.area}')`;
                }

                connection.query(query, (err, results) => {
                    if(err) reject(new Error(err.message));
                    resolve(results);
                });
            });

            console.log(response);
            return response;

        } catch (error) {
            console.log(error);
        }
    }
}

module.exports = DbService;
