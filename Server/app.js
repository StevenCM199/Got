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
var resetSource = "";

/**
 * Crea la conexión entre el servidor y la base de datos utilizando la informacion del host, usuario, la base de datos a la que se quiere conectar, la contraseña de la base de datos y el puerto al que se desea conectar
 * @example 
 *    const connection = mysql.createConnection({
      host: 'localhost',
      user: 'root',
      database: 'got',
      password: '1234',
      port: '3306',
      insecureAuth : true
      })
 * 
 * connection es también el miembro encargado de ejecutar las querys a la base de datos
 * 
*/
const connection = mysql.createConnection({
    host: 'localhost',
    user: 'root',
    database: 'got',
    password: 'Akari12',
    port: '3306',
    insecureAuth : true
})

app.use(morgan('combined'))

//POST user
app.post('/user', urlencodedParser, (req, res) =>{
  var date = getDate();
  connection.query("INSERT INTO usuario (nombre, fechaInicio) VALUES ('"+ req.body.nombre +"', '"+ date +"')", (err, rows, fields) =>{
      if (err){
          console.log(err);
      }
  })
  return res.send("Se ha iniciado sesion " + req.body.nombre + "\n")
})

/**
 * @module post/init
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @param req.body.nombre {String} El nombre del repositorio
 * @description Crea el repositorio y lo añade a la base de datos al recibir el comando "init"
 */
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

/**
 * @module post/commit
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @param date {Object} Dia y hora actuales
 * @param nombreRepo {Object} Nombre del repositorio actual
 * @param idRepo {Object} Id del repositorio en la base de datos 
 * @description Ejecuta un query que añade el commit al repositorio especificado 
 */
//POST request for commit
app.post('/commit', urlencodedParser, (req, res) =>{
    var date = getDate();
    var nombreRepo = req.body.nameRepo;
    var idRepo;
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
    connection.query('SELECT id, nombre FROM commit ORDER BY commit.id DESC', function(err, rows){
      var idCommit = rows[0].id;
      res.send("Se realizado el commit: " + MD5("" + idCommit) + "\n");
  })
    return 1;//res.send("Se ha realizado el commit: " + req.body.commitName + "\n")
})

/**
 * @module post/file
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @param idcommit {Object} Id del commit en la base de datos
 * @param commitName {Object} Nombre del commit al que se quiere subir el archivo
 * @param idRepo {Object} Id del repositorio en la base de datos 
 * @description Ejecuta un query que añade el archivo al commit especificado 
 */
app.post('/file', urlencodedParser, (req, res) =>{
    var commitName = req.body.commitName;
    var idCommit;
    console.log("Nombre: " + commitName);
    connection.query('SELECT * FROM commit WHERE nombre = ?', commitName, function(err, rows){
      if(err){
          console.log("Error: " + err)
        }  
        idCommit = rows[0].id;
        var idRepo = rows[0].repositorio;
    
        connection.query('SELECT COUNT(*) as cantidad FROM archivo WHERE absPath = ?', req.body.absolutePath, function(err,result){

          if(result[0].cantidad == 0){
            insertFirstFile(idCommit);
            
            }
          else{
            getPrevious(idCommit, idRepo); 
            }
        }) 
    })

    function getPrevious(currentCommit, idRepo){
        var previousCommit;
        connection.query('SELECT * FROM commit WHERE repositorio = ?', idRepo, function(err,rows) {
            //console.log(rows);
            previousCommit = rows[rows.length-2].id;
            getPreviousFile(currentCommit, previousCommit);
        })
    }

    function getPreviousFile(currentCommit, previousCommit){
      var conditions = [req.body.absolutePath, previousCommit]
      connection.query('SELECT * FROM archivo WHERE absPath = ? AND commit = ?', conditions , function(err,rows) {
        //console.log(rows);
        try {
          previousCommit = rows[0].id;
          insertFile(currentCommit, previousCommit);
        } catch (error) {
          
        }
        
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
  connection.query('SELECT DISTINCT archivo.absPath FROM archivo', function(err, rows){
    var message = "";
    for(var i = 0; i<rows.length; i++){
      message += rows[i].absPath + ',';
      connection.query('SELECT archivo.data FROM archivo WHERE absPath = ?', rows[i].absPath, function(err,rows){
        if(err){
          console.log("error: " + err);
        }
        var source = rows[0].data;
        source = Huffman.decode(source);
        for(var i = 1; i< rows.length; i++){
            //descomprimir diff
            var patch = rows[i].data;
            patch = Huffman.decode(patch);
            patch = JSON.parse(patch);
            source = jsDiff.applyPatch(source, patch);
        }
        console.log(source);
        addMessage(source);
      })
    }

    function addMessage(source){
      message += source + '&';
      res.send(message);
    }    
  })
  return 1;
})

/**
 * @module post/statusFile
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @description Devuelve el estado actual del archivo en el repositorio
 */
app.post('/statusFile', urlencodedParser, (req, res) =>{
    connection.query('SELECT c.nombre, a.data FROM commit AS c INNER JOIN archivo AS a ON a.commit = c.id WHERE a.absPath = ?', req.body.absolutePath, function(err, rows, result){
      if (err){
        console.log(err);
      }
      var source = rows[0].data;
      var message = "";
      for(var i = 0; i< rows.length; i++){
        //descomprimir diff
        message += rows[i].nombre + "\n";
        var data = rows[i].data;
        message += Huffman.decode(data) + "\n\n";
      }
      res.send(message);
    })
    return 1;
})

/**
 * @module post/rollback
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @description Devuelve el estado actual del archivo en el repositorio
 */
app.post('/rollback', urlencodedParser, (req, res) =>{
    connection.query('SELECT * FROM commit WHERE nombre = ?', req.body.commitName, function(err,rows){
      idCommit = rows[0].id;
      rollbackFile(idCommit);
    })

    function rollbackFile(idCommit){
      var conditions = [req.body.absolutePath, idCommit]
      connection.query('SELECT * FROM archivo WHERE absPath = ? AND commit <= ? ', conditions, function(err,rows){
        var source = rows[0].data;
        source = Huffman.decode(source);
        for(var i = 1; i< rows.length; i++){
          //descomprimir diff
          var patch = rows[i].data;
          patch = Huffman.decode(patch);
          patch = JSON.parse(patch);
          source = jsDiff.applyPatch(source, patch)
        }
        console.log("source rollback: " + source);
        res.send(source); 
      })
    }

    return 1
})

/**
 * @module post/reset
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @param source {Object} Cadena de texto del archivo 
 * @description Regresa los cambios locales a el ultimo commit subido al servidor
 */
app.post('/reset', urlencodedParser, (req, res) =>{
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
    console.log("source: " + source)
    setResetSource(source);
    res.send(source);    
  })
    console.log("result: " + resetSource)
    return 1 //res.send(resetSource)
})

function setResetSource(source){
  resetSource = source
}

/**
 * @module post/sync
 * @function
 * @param req {Object} Datos del cliente
 * @param res {Object} Respuesta del servidor
 * @param source {Object} Cadena de texto del archivo 
 * @description Actualiza los archivos locales al último commit realizado en el servidor
 */
app.post('/sync', urlencodedParser, (req, res) =>{
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
    console.log("source: " + source)
    res.send(source);    
  })
    console.log("result: " + resetSource);
    return 1
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

var MD5 = function (string) {

    function RotateLeft(lValue, iShiftBits) {
            return (lValue<<iShiftBits) | (lValue>>>(32-iShiftBits));
    }
 
    function AddUnsigned(lX,lY) {
            var lX4,lY4,lX8,lY8,lResult;
            lX8 = (lX & 0x80000000);
            lY8 = (lY & 0x80000000);
            lX4 = (lX & 0x40000000);
            lY4 = (lY & 0x40000000);
            lResult = (lX & 0x3FFFFFFF)+(lY & 0x3FFFFFFF);
            if (lX4 & lY4) {
                    return (lResult ^ 0x80000000 ^ lX8 ^ lY8);
            }
            if (lX4 | lY4) {
                    if (lResult & 0x40000000) {
                            return (lResult ^ 0xC0000000 ^ lX8 ^ lY8);
                    } else {
                            return (lResult ^ 0x40000000 ^ lX8 ^ lY8);
                    }
            } else {
                    return (lResult ^ lX8 ^ lY8);
            }
    }
 
    function F(x,y,z) { return (x & y) | ((~x) & z); }
    function G(x,y,z) { return (x & z) | (y & (~z)); }
    function H(x,y,z) { return (x ^ y ^ z); }
    function I(x,y,z) { return (y ^ (x | (~z))); }
 
    function FF(a,b,c,d,x,s,ac) {
            a = AddUnsigned(a, AddUnsigned(AddUnsigned(F(b, c, d), x), ac));
            return AddUnsigned(RotateLeft(a, s), b);
    };
 
    function GG(a,b,c,d,x,s,ac) {
            a = AddUnsigned(a, AddUnsigned(AddUnsigned(G(b, c, d), x), ac));
            return AddUnsigned(RotateLeft(a, s), b);
    };
 
    function HH(a,b,c,d,x,s,ac) {
            a = AddUnsigned(a, AddUnsigned(AddUnsigned(H(b, c, d), x), ac));
            return AddUnsigned(RotateLeft(a, s), b);
    };
 
    function II(a,b,c,d,x,s,ac) {
            a = AddUnsigned(a, AddUnsigned(AddUnsigned(I(b, c, d), x), ac));
            return AddUnsigned(RotateLeft(a, s), b);
    };
 
    function ConvertToWordArray(string) {
            var lWordCount;
            var lMessageLength = string.length;
            var lNumberOfWords_temp1=lMessageLength + 8;
            var lNumberOfWords_temp2=(lNumberOfWords_temp1-(lNumberOfWords_temp1 % 64))/64;
            var lNumberOfWords = (lNumberOfWords_temp2+1)*16;
            var lWordArray=Array(lNumberOfWords-1);
            var lBytePosition = 0;
            var lByteCount = 0;
            while ( lByteCount < lMessageLength ) {
                    lWordCount = (lByteCount-(lByteCount % 4))/4;
                    lBytePosition = (lByteCount % 4)*8;
                    lWordArray[lWordCount] = (lWordArray[lWordCount] | (string.charCodeAt(lByteCount)<<lBytePosition));
                    lByteCount++;
            }
            lWordCount = (lByteCount-(lByteCount % 4))/4;
            lBytePosition = (lByteCount % 4)*8;
            lWordArray[lWordCount] = lWordArray[lWordCount] | (0x80<<lBytePosition);
            lWordArray[lNumberOfWords-2] = lMessageLength<<3;
            lWordArray[lNumberOfWords-1] = lMessageLength>>>29;
            return lWordArray;
    };
 
    function WordToHex(lValue) {
            var WordToHexValue="",WordToHexValue_temp="",lByte,lCount;
            for (lCount = 0;lCount<=3;lCount++) {
                    lByte = (lValue>>>(lCount*8)) & 255;
                    WordToHexValue_temp = "0" + lByte.toString(16);
                    WordToHexValue = WordToHexValue + WordToHexValue_temp.substr(WordToHexValue_temp.length-2,2);
            }
            return WordToHexValue;
    };
 
    function Utf8Encode(string) {
            string = string.replace(/\r\n/g,"\n");
            var utftext = "";
 
            for (var n = 0; n < string.length; n++) {
 
                    var c = string.charCodeAt(n);
 
                    if (c < 128) {
                            utftext += String.fromCharCode(c);
                    }
                    else if((c > 127) && (c < 2048)) {
                            utftext += String.fromCharCode((c >> 6) | 192);
                            utftext += String.fromCharCode((c & 63) | 128);
                    }
                    else {
                            utftext += String.fromCharCode((c >> 12) | 224);
                            utftext += String.fromCharCode(((c >> 6) & 63) | 128);
                            utftext += String.fromCharCode((c & 63) | 128);
                    }
 
            }
 
            return utftext;
    };
 
    var x=Array();
    var k,AA,BB,CC,DD,a,b,c,d;
    var S11=7, S12=12, S13=17, S14=22;
    var S21=5, S22=9 , S23=14, S24=20;
    var S31=4, S32=11, S33=16, S34=23;
    var S41=6, S42=10, S43=15, S44=21;
 
    string = Utf8Encode(string);
 
    x = ConvertToWordArray(string);
 
    a = 0x67452301; b = 0xEFCDAB89; c = 0x98BADCFE; d = 0x10325476;
 
    for (k=0;k<x.length;k+=16) {
            AA=a; BB=b; CC=c; DD=d;
            a=FF(a,b,c,d,x[k+0], S11,0xD76AA478);
            d=FF(d,a,b,c,x[k+1], S12,0xE8C7B756);
            c=FF(c,d,a,b,x[k+2], S13,0x242070DB);
            b=FF(b,c,d,a,x[k+3], S14,0xC1BDCEEE);
            a=FF(a,b,c,d,x[k+4], S11,0xF57C0FAF);
            d=FF(d,a,b,c,x[k+5], S12,0x4787C62A);
            c=FF(c,d,a,b,x[k+6], S13,0xA8304613);
            b=FF(b,c,d,a,x[k+7], S14,0xFD469501);
            a=FF(a,b,c,d,x[k+8], S11,0x698098D8);
            d=FF(d,a,b,c,x[k+9], S12,0x8B44F7AF);
            c=FF(c,d,a,b,x[k+10],S13,0xFFFF5BB1);
            b=FF(b,c,d,a,x[k+11],S14,0x895CD7BE);
            a=FF(a,b,c,d,x[k+12],S11,0x6B901122);
            d=FF(d,a,b,c,x[k+13],S12,0xFD987193);
            c=FF(c,d,a,b,x[k+14],S13,0xA679438E);
            b=FF(b,c,d,a,x[k+15],S14,0x49B40821);
            a=GG(a,b,c,d,x[k+1], S21,0xF61E2562);
            d=GG(d,a,b,c,x[k+6], S22,0xC040B340);
            c=GG(c,d,a,b,x[k+11],S23,0x265E5A51);
            b=GG(b,c,d,a,x[k+0], S24,0xE9B6C7AA);
            a=GG(a,b,c,d,x[k+5], S21,0xD62F105D);
            d=GG(d,a,b,c,x[k+10],S22,0x2441453);
            c=GG(c,d,a,b,x[k+15],S23,0xD8A1E681);
            b=GG(b,c,d,a,x[k+4], S24,0xE7D3FBC8);
            a=GG(a,b,c,d,x[k+9], S21,0x21E1CDE6);
            d=GG(d,a,b,c,x[k+14],S22,0xC33707D6);
            c=GG(c,d,a,b,x[k+3], S23,0xF4D50D87);
            b=GG(b,c,d,a,x[k+8], S24,0x455A14ED);
            a=GG(a,b,c,d,x[k+13],S21,0xA9E3E905);
            d=GG(d,a,b,c,x[k+2], S22,0xFCEFA3F8);
            c=GG(c,d,a,b,x[k+7], S23,0x676F02D9);
            b=GG(b,c,d,a,x[k+12],S24,0x8D2A4C8A);
            a=HH(a,b,c,d,x[k+5], S31,0xFFFA3942);
            d=HH(d,a,b,c,x[k+8], S32,0x8771F681);
            c=HH(c,d,a,b,x[k+11],S33,0x6D9D6122);
            b=HH(b,c,d,a,x[k+14],S34,0xFDE5380C);
            a=HH(a,b,c,d,x[k+1], S31,0xA4BEEA44);
            d=HH(d,a,b,c,x[k+4], S32,0x4BDECFA9);
            c=HH(c,d,a,b,x[k+7], S33,0xF6BB4B60);
            b=HH(b,c,d,a,x[k+10],S34,0xBEBFBC70);
            a=HH(a,b,c,d,x[k+13],S31,0x289B7EC6);
            d=HH(d,a,b,c,x[k+0], S32,0xEAA127FA);
            c=HH(c,d,a,b,x[k+3], S33,0xD4EF3085);
            b=HH(b,c,d,a,x[k+6], S34,0x4881D05);
            a=HH(a,b,c,d,x[k+9], S31,0xD9D4D039);
            d=HH(d,a,b,c,x[k+12],S32,0xE6DB99E5);
            c=HH(c,d,a,b,x[k+15],S33,0x1FA27CF8);
            b=HH(b,c,d,a,x[k+2], S34,0xC4AC5665);
            a=II(a,b,c,d,x[k+0], S41,0xF4292244);
            d=II(d,a,b,c,x[k+7], S42,0x432AFF97);
            c=II(c,d,a,b,x[k+14],S43,0xAB9423A7);
            b=II(b,c,d,a,x[k+5], S44,0xFC93A039);
            a=II(a,b,c,d,x[k+12],S41,0x655B59C3);
            d=II(d,a,b,c,x[k+3], S42,0x8F0CCC92);
            c=II(c,d,a,b,x[k+10],S43,0xFFEFF47D);
            b=II(b,c,d,a,x[k+1], S44,0x85845DD1);
            a=II(a,b,c,d,x[k+8], S41,0x6FA87E4F);
            d=II(d,a,b,c,x[k+15],S42,0xFE2CE6E0);
            c=II(c,d,a,b,x[k+6], S43,0xA3014314);
            b=II(b,c,d,a,x[k+13],S44,0x4E0811A1);
            a=II(a,b,c,d,x[k+4], S41,0xF7537E82);
            d=II(d,a,b,c,x[k+11],S42,0xBD3AF235);
            c=II(c,d,a,b,x[k+2], S43,0x2AD7D2BB);
            b=II(b,c,d,a,x[k+9], S44,0xEB86D391);
            a=AddUnsigned(a,AA);
            b=AddUnsigned(b,BB);
            c=AddUnsigned(c,CC);
            d=AddUnsigned(d,DD);
            }
 
        var temp = WordToHex(a)+WordToHex(b)+WordToHex(c)+WordToHex(d);
 
        return temp.toLowerCase();
 }