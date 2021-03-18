var myJsonDatas = new Array();
//console.log("length: "+myJsonDatas.length);
/*
$.getJSON( "data", function( myJson ) {
    //console.log(myJson);
    
    for (let index = 0; index < myJson.length; index++) {
        myJsonDatas.push(myJson[index]);
        
    }
    console.log(myJsonDatas);
});*/
//console.log(myJsonDatas);




/* Teszt olvasás */
// Retrieving data:
text = localStorage.getItem("testJSON");
obj = JSON.parse(text);
console.log(obj);
//labelsArray feltöltése a kiolvasott adatokkal
var labelArray = new Array();
    obj.forEach(element => {
    labelArray.push(element.name);
    //labelArray.push(element.time/1000/60);
});
//dataArray feltöltése a kiolvasott adatokkal
var dataArray = new Array();
    obj.forEach(element => {
    dataArray.push(element.vote);
    //dataArray.push(element.T1);
});

var ctx = document.getElementById('myChart');
var myChart = new Chart(ctx, {
    type: 'bar',
    data: {
        //labels: ['Red', 'Blue', 'Yellow', 'Green', 'Purple', 'Orange'],
        //Labels megkapja a labelArray-t
        labels: labelArray,
        
        datasets: [{
            label: '# of Temp',
            //data: [12, 19, 3, 5, 2, 3],
            //data megkapja a dataArray-t
            data: dataArray,
            backgroundColor: [
                'rgba(255, 99, 132, 0.2)',
                'rgba(54, 162, 235, 0.2)',
                'rgba(255, 206, 86, 0.2)',
                'rgba(75, 192, 192, 0.2)',
                'rgba(153, 102, 255, 0.2)',
                'rgba(255, 159, 64, 0.2)'
            ],
            borderColor: [
                'rgba(255, 99, 132, 1)',
                'rgba(54, 162, 235, 1)',
                'rgba(255, 206, 86, 1)',
                'rgba(75, 192, 192, 1)',
                'rgba(153, 102, 255, 1)',
                'rgba(255, 159, 64, 1)'
            ],
            borderWidth: 1
        }]
    },
    options: {
        scales: {
            yAxes: [{
                ticks: {
                    beginAtZero: true
                }
            }]
        }
    }
});





function addData(chart, label, data) {
    chart.data.labels.push(label);
    chart.data.datasets.forEach((dataset) => {
        dataset.data.push(data);
    });
    chart.update();
}

function removeData(chart) {
    chart.data.labels.pop();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.pop();
    });
    chart.update();
}

function removeAllData(chart) {
    while (chart.data.labels.length > 0) {
        chart.data.labels.pop();
        chart.data.datasets.forEach((dataset) => {
        dataset.data.pop();
    });
    chart.update();    
    }
}

function addLotOfData(chart, newLabels, newDatas){
    for (let i = 0; i < newLabels.length; i++) {
        addData(chart,newLabels[i],newDatas[i]);
    }
}

var peti = 'Peti';
var tizenot = 15;
addData(myChart, peti, tizenot);

var label2 = ['red','blue','green'];
var data2 = [10, 20, 15];



var getJSON = function(url) {
    return new Promise(
      function(resolve, reject) 
      {
          var xhr = new XMLHttpRequest();
          xhr.open('get', url, true);
          xhr.responseType = 'json';
          xhr.onload = function() 
          {
              var status = xhr.status;
              if (status == 200) {
                  resolve(xhr.response);
              } 
              else {
                  reject(status);
              }
          };
          xhr.send();
      });
  };
  //http://127.0.0.1/data
  getJSON("data").then(function(data) {
      myJsonDatas = data;
      console.log(myJsonDatas);
      removeAllData(myChart);
      var myJDtimes = new Array();
      var myJDtemp = new Array();
      for (let i = 0; i < myJsonDatas.length; i++) {
          myJDtimes.push(milisecToMin(myJsonDatas[i].time));
          myJDtemp.push(myJsonDatas[i].T1);
      }
      addLotOfData(myChart,myJDtimes, myJDtemp);
  }, function(status) { //error detection....
    alert('Something went wrong.');
  });


  function milisecToMin(milisec){
    return min = milisec/1000/60;
  }