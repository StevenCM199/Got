// server using express
const express = require('express')
const app = express()
const morgan = require('morgan')
const mysql = require('mysql')
var actualRepo = "" 
var bodyParser = require('body-parser')
var jsonParser = bodyParser.json()
var urlencodedParser = bodyParser.urlencoded({ extended: false })
var jsDiff = require('diff');
//import {encode} from "./huffman.js";
//import {decode} from "./huffman.js";

const connection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    database: 'got',
    password: 'Pikasha199',
    port: '3306',
    insecureAuth : true
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

//POST request for init
app.post('/init', urlencodedParser, (req, res) =>{
    connection.query("INSERT INTO repositorio (nombre) VALUES ('"+ req.body.nombre +"')", (err, rows, fields) =>{
    console.log("init works!")
        if (err){
            console.log(err);
        }
    })
    return res.send("Se ha creado el repositorio " + req.body.nombre + "\n")
})

//POST request for commit
app.post('/commit', urlencodedParser, (req, res) =>{
    var date = getDate();
    var nombreRepo = req.body.nameRepo;
    var idRepo;
    console.log(nombreRepo);
    console.log(req.body.data);
    connection.query('SELECT id, nombre FROM repositorio WHERE nombre = ?', nombreRepo, function(err, rows){
        idRepo = rows[0].id;
        insertCommit(idRepo);
    })

    function insertCommit(idRepo){
        connection.query("INSERT INTO commit (nombre, fecha, repositorio) VALUES ('"+ req.body.commitName +"', '"+ date +"', '"+ idRepo +"')", (err, rows, fields) =>{
            console.log("commit works!")
                if (err){
                    console.log(err);
                }
            })
    }
    return res.send("Se ha realizado el commit: " + req.body.commitName + "\n")
})

app.post('/file', urlencodedParser, (req, res) =>{
    var commitName = req.body.commitName;
    var idCommit;
    console.log(commitName);
    connection.query('SELECT * FROM commit WHERE nombre = ?', commitName, function(err, rows){
      if(err){
          console.log("Error: " + err)
        }  
        idCommit = rows[0].id;
        var idRepo = rows[0].repositorio;
        console.log(idRepo);
        console.log(req.body.absolutePath)

        
        
        connection.query('SELECT COUNT(*) as cantidad FROM archivo WHERE absPath = ?', req.body.absolutePath, function(err,result){
          console.log(result);
          console.log(result[0].cantidad);

          if(result[0].cantidad == 0){
            console.log("firstFile")
            insertFirstFile(idCommit);
            
            }
          else{
            console.log("previous")
            getPrevious(idCommit, idRepo); 
            }
        }) 
    })

    function getPrevious(currentCommit, idRepo){
        var previousCommit;
        console.log(idRepo);
        connection.query('SELECT * FROM commit WHERE repositorio = ?', idRepo, function(err,rows) {
            console.log(rows);
            previousCommit = rows[rows.length-2].id;
            getPreviousFile(currentCommit, previousCommit);
        })
    }

    function getPreviousFile(currentCommit, previousCommit){
      var conditions = [req.body.absolutePath, previousCommit]
      connection.query('SELECT * FROM archivo WHERE absPath = ? AND commit = ?', conditions , function(err,rows) {
        console.log(rows);
        previousCommit = rows[0].id;
        insertFile(currentCommit, previousCommit);
    })
    }

    function insertFirstFile(idCommit) {
        var data = Huffman.encode(req.body.data);
        connection.query("INSERT INTO archivo (nombre, commit, data, absPath) VALUES ('"+ req.body.file +"', '"+ idCommit+"', '"+ data +"', '"+ req.body.absolutePath +"' )", (err, rows, fields) =>{
            console.log("archivo insertado")
                if (err){
                    console.log(err);
                }
            })
    }

    function insertFile(idCommit, idPreviousCommit){
        // diff
        connection.query('SELECT * FROM archivo WHERE absPath = ?', req.body.absolutePath, function(err, rows){
            var source = rows[0].data;
            source = Huffman.decode(source);
            for(var i = 1; i< rows.length; i++){
                //descomprimir diff
                var patch = rows[i].data;
                patch = Huffman.decode(patch);
                patch = JSON.parse(patch);
                source = jsDiff.applyPatch(source, patch)
            }
            
            console.log("Source: \n" + source + "\n\n");
            var diff = jsDiff.createPatch(req.body.file, source, req.body.data);
            var parsed = jsDiff.parsePatch(diff);
            var parsedStringify = JSON.stringify(parsed);
            console.log("ParsedStringify: \n" + parsedStringify + "\n\n");
            var encoded = Huffman.encode(parsedStringify);
            setFile(encoded, idPreviousCommit, idCommit);
        })
    }
    function setFile(data, idPreviousCommit, idCommit) {
        connection.query("INSERT INTO archivo (nombre, commit, data, anterior, absPath) VALUES ('"+ req.body.file +"', '"+ idCommit+"', '"+ data +"', '"+ idPreviousCommit +"', '"+ req.body.absolutePath +"')", (err, rows, fields) =>{
                if (err){
                    console.log(err);
                }
            })
    }
    return res.send("Se ha agregado el archivo\n")
})

function getDate(){
    var today = new Date();
    var dd = String(today.getDate()).padStart(2, '0');
    var mm = String(today.getMonth() + 1).padStart(2, '0'); //January is 0!
    var yyyy = today.getFullYear();

    today = mm + '/' + dd + '/' + yyyy;
    return today
}

app.post('/status', urlencodedParser, (req, res) =>{
    
    return res.send("Status")
})

app.post('/rollback', urlencodedParser, (req, res) =>{
    
    return res.send("rollback")
})

app.post('/reset', urlencodedParser, (req, res) =>{
    
    return res.send("reset")
})

app.post('/sync', urlencodedParser, (req, res) =>{
    
    return res.send("sync")
})

//PUT request
app.put('/', (req, res) =>{
    return res.send("Received a PUT HTTP method")
})

//DELETE request
app.delete('/', (req, res) => {
    return res.send('Received a DELETE HTTP method');
  });


// localhost:3002
app.listen(3002, () =>{
    console.log("Sever is up and listening on 3002...")
})

// Huffman

log = console.log.bind(console);

var Heap = function(fn) {
  this.fn = fn || function(e) {
    return e;
  };
  this.items = [];
};

Heap.prototype = {
  swap: function(i, j) {
    this.items[i] = [
      this.items[j],
      this.items[j] = this.items[i]
    ][0];
  },
  bubble: function(index) {
    var parent = ~~((index - 1) / 2);
    if (this.item(parent) < this.item(index)) {
      this.swap(index, parent);
      this.bubble(parent);
    }
  },
  item: function(index) {
    return this.fn(this.items[index]);
  },
  pop: function() {
    return this.items.pop();
  },
  sift: function(index, end) {
    var child = index * 2 + 1;
    if (child < end) {
      if (child + 1 < end && this.item(child + 1) > this.item(child)) {
        child++;
      }
      if (this.item(index) < this.item(child)) {
        this.swap(index, child);
        return this.sift(child, end);
      }
    }
  },
  push: function() {
    var lastIndex = this.items.length;
    for (var i = 0; i < arguments.length; i++) {
      this.items.push(arguments[i]);
      this.bubble(lastIndex++);
    }
  },
  get length() {
    return this.items.length;
  }
};

var Huffman = {
  encode: function(data) {
    var prob = {};
    var tree = new Heap(function(e) {
      return e[0];
    });
    for (var i = 0; i < data.length; i++) {
      if (prob.hasOwnProperty(data[i])) {
        prob[data[i]]++;
      } else {
        prob[data[i]] = 1;
      }
    }
    Object.keys(prob).sort(function(a, b) {
      return ~~(Math.random() * 2);
    }).forEach(function(e) {
      tree.push([prob[e], e]);
    });
    while (tree.length > 1) {
      var first = tree.pop(),
          second = tree.pop();
      tree.push([first[0] + second[0], [first[1], second[1]]]);
    }
    var dict = {};
    var recurse = function(root, string) {
      if (root.constructor === Array) {
        recurse(root[0], string + '0');
        recurse(root[1], string + '1');
      } else {
        dict[root] = string;
      }
    };
    tree.items = tree.pop()[1];
    recurse(tree.items, '');
    var result = '';
    for (var i = 0; i < data.length; i++) {
      result += dict[data.charAt(i)];
    }
    var header = Object.keys(dict).map(function(e) {
      return e.charCodeAt(0) + '|' + dict[e];
    }).join('-') + '/';
    return header + result;
  },
  decode: function(string) {
    string = string.split('/');
    var data = string[1].split(''),
        header = {};
    string[0].split('-').forEach(function(e) {
      var values = e.split('|');
      header[values[1]] = String.fromCharCode(values[0]);
    });
    var result = '';
    while (data.length) {
      var i = 0,
          cur = '';
      while (data.length) {
        cur += data.shift();
        if (header.hasOwnProperty(cur)) {
          result += header[cur];
          break;
        }
      }
    }
    return result;
  }
};