var myJsonDatas = new Array();
var labelArray = new Array();
var dataArray = new Array();

//teszt adatok:
var label2 = ['red', 'blue', 'green'];
var data2 = [10, 20, 15];

var ctx = document.getElementById('myChart');
var myChart = new Chart(ctx, {
    type: 'line',
    data: {
        labels: labelArray,
        datasets: [{
            label: '1st temp',
            data: dataArray,
            backgroundColor: [
                'rgba(255, 255, 255, 0.2)'
            ],
            borderColor: [
                'rgba(255, 99, 132, 1)'
            ],
            borderWidth: 1
        },{
            label: '2nd temp',
            data: [20, 30, 20, 30],
            backgroundColor: [
                'rgba(255, 255, 255, 0.2)'
            ],
            borderColor: [
                'rgba(83, 51, 237, 1)'
            ],
            borderWidth: 1
        },{
            label: 'set temp',
            data: [24, 24, 24, 24],
            backgroundColor: [
                'rgba(255, 255, 255, 0.2)'
            ],
            borderColor: [
                'rgba(0, 0, 0, 0)'
            ],
            borderWidth: 1
        },
    
        ]
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

function removeFirstData(chart) {
    chart.data.labels.shift();
    chart.data.datasets.forEach((dataset) => {
        dataset.data.shift();
    });
    chart.update();
}

function addLotOfData(chart, newLabels, newDatas) {
    for (let i = 0; i < newLabels.length; i++) {
        addData(chart, newLabels[i], newDatas[i]);
    }
}

function refres(){
    //TODO
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
    return [year, month, day, hour, min, sec].join('-');
}

function currentMeasurementDateFunction(myJsonDatas, i){
    //rendszer ideje
    var sysDate = new Date();
    var sysMiliSec = sysDate.getTime();
    //return: rendszeridő - (utolsó mérés - aktuális mérés) azaz msot hoz képest az aktuális mérés mikor történt (milisec)
    return currentMeasurementDate = new Date(sysMiliSec - (myJsonDatas[myJsonDatas.length - 1].time - myJsonDatas[i].time));
}


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
//http://127.0.0.1/data
getJSON("data").then(function (data) {
    myJsonDatas = data;
    console.log(myJsonDatas);
    removeAllData(myChart);
    var myJDtimes = new Array();
    var myJDtemp = new Array();

    for (let i = 0; i < myJsonDatas.length; i++) {
        myJDtimes.push(formatDate(currentMeasurementDateFunction(myJsonDatas,i)));
        myJDtemp.push(myJsonDatas[i].T1);
    }
    addLotOfData(myChart, myJDtimes, myJDtemp);
}, function (status) { //error detection....
    alert('Something went wrong.');
});

console.log("aaaaaaa");

