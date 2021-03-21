var myJsonDatas = new Array();
var labelArray = new Array();
var dataArray = new Array();

var ctx = document.getElementById('myChart');
var myChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: labelArray,
        datasets: [{
            label: '# of Temp',
            data: dataArray,
            //borderColor: [rgba(255,99,132,0.2)],
            backgroundColor: [
                'rgba(255, 255, 255, 0.2)'
            ],
            /*  'rgba(255, 99, 132, 0.2)',
                'rgba(54, 162, 235, 0.2)',
                'rgba(255, 206, 86, 0.2)',
                'rgba(75, 192, 192, 0.2)',
                'rgba(153, 102, 255, 0.2)',
                'rgba(255, 159, 64, 0.2)'*/
            borderColor: [
                'rgba(255, 99, 132, 1)'
            ],
            /*  ,'rgba(54, 162, 235, 1)',
                'rgba(255, 206, 86, 1)',
                'rgba(75, 192, 192, 1)',
                'rgba(153, 102, 255, 1)',
                'rgba(255, 159, 64, 1)'*/
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
      console.log(data);
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
  }
);


function milisecToMin(milisec){
    return min = milisec/1000/60;
}
