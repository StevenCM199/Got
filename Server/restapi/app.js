// server using express
const express = require('express')
const app = express()
const morgan = require('morgan')
const mysql = require('mysql')

app.use(morgan('combined'))

app.get('/user/:id', (req, res) =>{
    console.log("Fetching user with id: " + req.params.id)

    const connection = mysql.createConnection({
        host: 'localhost',
        user: 'root',
        database: 'got'
    })

    connection.query("SELECT * FROM commit", (err, rows, fields) =>{
        console.log("I think it works!")
        res.json(rows)
    })

    //res.end()
})

// GET request
app.get("/", (req, res) => {
    console.log("Responding to root route")
    res.send("Hello from root")
})

app.get("/users", (req, res) =>{
    // responds with a json
    var user1 = {firstName: "Stephen", lastName: "Curry"}
    const user2 = {firstName: "Kevin", lastName: "Durant"}
    res.json([user1, user2])
    //res.send("Nodemon auto update")
})

//POST request
app.post('/', (req, res) =>{
    return res.send("Received a POST HTTP method")
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