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
        document.getElementById("settemp").innerHTML = this.responseText;
      }
    };
    xhttp.open("GET", "/settemp", true);
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
