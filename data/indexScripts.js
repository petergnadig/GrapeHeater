setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("temperature1").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/temperature1", true);
    xhttp.send();
  }, 10000 ) ;

  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("temperature2").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/temperature2", true);
    xhttp.send();
  }, 10000 ) ;
  setInterval(function ( ) {
    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function() {
      if (this.readyState == 4 && this.status == 200) {
        document.getElementById("heating").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/heating", true);
    xhttp.send();
  }, 10000 ) ;

/*
 // Storing test data:
 myObj1 = {name: "John", vote: 31, city: "New York"};
 myObj2 = {name: "Bela", vote: 42, city: "Budapest"};
 myObj3 = {name: "Tamas", vote: 52, city: "Kaposvar"};
 let myTestArray = [myObj1, myObj2, myObj3];
 myJSON = JSON.stringify(myTestArray);
 localStorage.setItem("testJSON", myJSON);
*/
