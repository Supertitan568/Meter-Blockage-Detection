function get_sensor_state(){
  let sensor_state_element = document.getElementById("sensor-state");
  fetch('/sensor').then(response => response.text().then(body => {
    if(body == "1"){
      sensor_state_element.innerText = "Movement Detected";
    }
    else{
      sensor_state_element.innerText = "Movenment Not Detected";
    }
  }));
}

setInterval(() => {
  get_sensor_state(); 
}, 5);
