// server using express
const express = require('express')
const app = express()
const morgan = require('morgan')
const mysql = require('mysql')
var actualRepo = "" 
var bodyparser = require('body-parser')


const connection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    database: 'got',
    password: '//contrasena'
})

app.use(morgan('combined'))

app.get('/user/:id', (req, res) =>{
    console.log("Fetching user with id: " + req.params.id)
    //res.end()
})

// GET request
app.get("/", (req, res) => {
    console.log("Responding to root route")
    res.send("Hello from root")
})

/*app.get("/users", (req, res) =>{
    // responds with a json
    var user1 = {firstName: "Stephen", lastName: "Curry"}
    const user2 = {firstName: "Kevin", lastName: "Durant"}
    res.json([user1, user2])
    //res.send("Nodemon auto update")
})*/

//POST request
app.post('/init', (req, res) =>{
    console.log("req: " + req + "\n");
  
    console.log("req.body: " + req.params.nombre + "\n");
    var repo = req.query;

    console.log("repo: " + repo + "\n");
    connection.query("INSERT INTO repositorio (nombre) VALUES ('"+ repo +"')", (err, rows, fields) =>{
        console.log("I think it works!")
        if (err){
            console.log(err);
        }
    })
    return res.send("Se ha creado el repositorio")
})

//PUT request
app.put('/', (req, res) =>{
    return res.send("Received a PUT HTTP method")
})

//DELETE request
app.delete('/', (req, res) => {
    return res.send('Received a DELETE HTTP method');
  });


// localhost:3003
app.listen(3003, () =>{
    console.log("Sever is up and listening on 3003...")
})

/* otra forma de app.listen
app.listen(process.env.PORT, () =>
  console.log(`Example app listening on port ${process.env.PORT}!`),
);*/