log = console.log.bind(console);

//Constructor del arbol
var Heap = function(fn) {
  this.fn = fn || function(e) {
    return e;
  };
  this.items = [];
};

Heap.prototype = {

  //Intercambiar de posicion los elementos
  swap: function(i, j) {
    this.items[i] = [
      this.items[j],
      this.items[j] = this.items[i]
    ][0];
  },
  //Reacomoda los indices del array con cada push
  bubble: function(index) {
    var parent = ~~((index - 1) / 2); //Toma el indice menos 1 a la mitad y redondea hacia abajo
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
  //Introduce un elemento al array
  push: function() {
    var lastIndex = this.items.length;
    for (var i = 0; i < arguments.length; i++) {
      this.items.push(arguments[i]);
      this.bubble(lastIndex++); //Reacomodar los indices
    }
  },
  get length() {
    return this.items.length;
  }
};

var Huffman = {
  //Crear el arbol
  //Se maneja como un array tipo Heap
  //Se define una probabilidad
  encode: function(data) {
    var prob = {};
    var tree = new Heap(function(e) {
      return e[0];
    });

    //Recorre el string para identificar los caracteres y sumar sus frecuencias
    for (var i = 0; i < data.length; i++) { 
      if (prob.hasOwnProperty(data[i])) {
        prob[data[i]]++;
      } else {
        prob[data[i]] = 1;
      }
    }
    //Introduce todos los caracteres y sus frecuencias al array
    Object.keys(prob).sort(function(a, b) { 
      return ~~(Math.random() * 2); 
    }).forEach(function(e) {
      tree.push([prob[e], e]); 
    });

    while (tree.length > 1) {
      var first = tree.pop(), //Saca los dos ultimos caracteres (de menor valor en la lista)
          second = tree.pop();
      tree.push([first[0] + second[0], [first[1], second[1]]]); //Crea un nodo padre de los ultimos
      //dos caracters de la lista, este tiene un valor de first + second y tiene a first y second como hijos
    }
    
    //Crea un diccionario con los codigos de los caracteres
    //Toma cada caracter del arbol y lo recorre desde abajo hacia arriba
    var dict = {};
    var recurse = function(root, string) {
      if (root.constructor === Array) {
        recurse(root[0], string + '0');
        recurse(root[1], string + '1');
      } else {
        dict[root] = string; //Si no encuentra un padre arriba, guarda el string del codigo
      } 
    };

    //Prepara el codigo binario del output usando el diccionario creado
    tree.items = tree.pop()[1];
    recurse(tree.items, '');
    var result = '';
    for (var i = 0; i < data.length; i++) {
      result += dict[data.charAt(i)];
    }
    //Prepara el diccionario en el formato codigo ASCII del caracter | codigo binario de huffman - ... 
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

var enc = Huffman.encode('TESTTESTTESTabccccccccccccc');
log(enc);
var dec = Huffman.decode(enc);
log(dec);