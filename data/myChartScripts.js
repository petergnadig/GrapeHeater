var myJsonDatas = new Array();
var labelArray = new Array();

var ctx = document.getElementById('myChart');

//let gif= 'loading.gif';
//$('#gif').attr('src', gif);

//var myGif =new GIF();
//myGif.load("loading-buffering.gif");
//ctx.drawImage(myGif.image,0,0); // will draw the playing gif image

//ctx.drawImage(gif.src,0,0);


var myChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: labelArray,
        datasets: 
        [
            {
                label: '1st temp',
                data: [],
                backgroundColor: ['rgba(255, 255, 255, 0.2)'],
                borderColor: ['rgba(255, 99, 132, 1)'],
                borderWidth: 1,
                pointRadius: 0
            },
            {
                label: '2nd temp',
                data: [],
                backgroundColor: ['rgba(255, 255, 255, 0.2)'],
                borderColor: ['rgba(83, 51, 237, 1)'],
                borderWidth: 1,
                pointRadius: 0
            },
            {
                label: 'set temp',
                data: [],
                backgroundColor: ['rgba(255, 255, 255, 0.2)'],
                borderColor: ['rgba(0, 0, 0, 0.2)'],
                borderWidth: 1,
                pointRadius: 0
            },
            {
                label: 'Heating',
                data: [],
                backgroundColor: ['rgba(255, 255, 255, 0.2)'],
                borderColor: ['rgba(0, 255, 0, 0.5)'],
                borderWidth: 1,
                pointRadius: 0,
                yAxisID: 'B'
            },
        ]
    },
    options: {
        scales: {
            yAxes: 
            [
                {
                    id: 'A',
                    type: 'linear',
                    position: 'left',
                    ticks: 
                    {
                        beginAtZero: true
                    }
                },
                {
                    id: 'B',
                    type: 'linear',
                    position: 'right',
                    ticks: 
                    {
                        max: 2,
                        min: 0
                    },
                    display: false
                }
            ]
        }
    }
});



function chartUpdate(chart){
    chart.update();
    
}

function addOneData(chart, label, data) {
    chart.data.labels.push(label);
    chart.data.datasets.forEach((dataset) => {
        dataset.data.push(data);
    });
    chart.update();
}

function addLabelData(chart, label){
    chart.data.labels.push(label);
    
}
function addDataData(chart, data, lineIndex){
    chart.data.datasets[lineIndex].data.push(data);
    
}

function addLotOfLabelData(chart, newLabels){
    for (let i = 0; i < newLabels.length; i++) {
        addLabelData(chart, newLabels[i]);
    }
}
function addLotOfDataData(chart, newData, lineIndex){
    for (let i = 0; i < newData.length; i++) {
        addDataData(chart, newData[i], lineIndex);
    }
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

function removeFirstData(chart) {
    chart.data.labels.shift();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.shift();
    });
    chart.update();
}



function formatDate(date) {
    var d = new Date(date);
    var sec = '' + d.getSeconds();
    var min = '' + d.getMinutes();
    var hour = '' + d.getHours();
    var day = '' + d.getDate();
    var month = '' + (d.getMonth() + 1);
    var year = d.getFullYear();
    //moth = 3 -> 03
    if (month.length < 2)
        month = '0' + month;
    if (day.length < 2)
        day = '0' + day;
    if (hour.length < 2)
        hour = '0' + hour;
    if (min.length < 2)
        min = '0' + min;
    if (sec.length < 2)
        sec = '0' + sec;
    return [ month, day, hour, min, sec].join('-');
}

function currentMeasurementDateFunction(myJsonDatas, i){
    //rendszer ideje
    var sysDate = new Date();
    var sysMiliSec = sysDate.getTime();
    //return: rendszeridő - (utolsó mérés - aktuális mérés) azaz msot hoz képest az aktuális mérés mikor történt (milisec)
    return currentMeasurementDate = new Date(sysMiliSec - (myJsonDatas[myJsonDatas.length - 1].time - myJsonDatas[i].time));
}
function removeGif() {
    var myobj = document.getElementById("gif");
    myobj.remove();
  }

  function createGif() {
    var x = document.createElement("img");
    x.setAttribute("id","gif");
    x.setAttribute("src", "loading.gif");
    x.setAttribute("alt", "Loading");
    document.body.appendChild(x);
  }

function UpdateChartNewDatas(){

    var getJSON = function (url) {
        return new Promise(
            function (resolve, reject) {
                var xhr = new XMLHttpRequest();
                xhr.open('get', url, true);
                xhr.responseType = 'json';
                xhr.onload = function () {
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

    getJSON("data").then(function (data) {
        myJsonDatas = data;
        removeAllData(myChart);
        var myJDtimes = new Array();
        var myJDtempT1 = new Array();
        var myJDtempT2 = new Array();
        var myJDtempST = new Array();
        var myJDHE = new Array();
    
        for (let i = 0; i < myJsonDatas.length; i++) {
            myJDtimes.push(formatDate(currentMeasurementDateFunction(myJsonDatas,i)));
            myJDtempT1.push(myJsonDatas[i].T1);
            myJDtempT2.push(myJsonDatas[i].T2);
            myJDtempST.push(myJsonDatas[i].ST);
            myJDHE.push(myJsonDatas[i].HE);
        }
        addLotOfLabelData(myChart, myJDtimes);
        addLotOfDataData(myChart, myJDtempT1, 0);
        addLotOfDataData(myChart, myJDtempT2, 1);
        addLotOfDataData(myChart, myJDtempST, 2);
        addLotOfDataData(myChart, myJDHE, 3);
        chartUpdate(myChart);
        //setTimeout(() => {  console.log("World!"); }, 5000);
        //setTimeout(() => removeGif(), 5000);
        removeGif();
    }, function (status) {
        alert('Something went wrong.');
    });
}

function UpdateButton(){
    createGif();
    UpdateChartNewDatas();
}

UpdateChartNewDatas();
