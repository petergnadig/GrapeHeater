  /*
  // Storing test data:
  myObj1 = {name: "John", age: 31, city: "New York"};
  myObj2 = {name: "Bela", age: 42, city: "Budapest"};
  myObj3 = {name: "Tamas", age: 52, city: "Kaposvar"};
  let myTestArray = [myObj1, myObj2, myObj3];
  myJSON = JSON.stringify(myTestArray);
  localStorage.setItem("testJSON", myJSON);
*/

// Retrieving data:
text = localStorage.getItem("testJSON");
obj = JSON.parse(text);
//document.getElementById("demo").innerHTML = obj.name;

obj.forEach((item, index) => {
    $(`#adatok`).append(
    `<ul class="box" data-index="${index}">
        <p> ${index}. adat </p>
        <li class="title">name: ${obj[index].name}</li>
        <li class="title">votes: ${obj[index].vote}</li>
        <li class="title">city: ${obj[index].city}</li>
        
     </ul>`
    );
});
