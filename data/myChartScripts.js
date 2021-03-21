var myJsonDatas = new Array();

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
//teszt töltéshez adatok
var peti = 'Peti';
var tizenot = 15;
addData(myChart, peti, tizenot);

var label2 = ['red', 'blue', 'green'];
var data2 = [10, 20, 15];

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
getJSON("data2").then(function (data) {
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


