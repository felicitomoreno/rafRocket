const fetchData = (module, accion, parameters) => {
    return new Promise( (resolve, reject) =>{
        const url = "https://serverall.000webhostapp.com/"
        const json = {
            'module': module,
            'accion': accion,
            'parameters': parameters,
        }
        const data = new FormData()
        data.append('json', JSON.stringify(json))
        const req = new XMLHttpRequest()
        req.open("post", url)
        req.onload = () => {
            (req.status === 200)
            ? resolve(req.responseText)
            : reject(req.error) 
        }
        req.send(data)
    })
}

const getData = async() => {
    try {
        let response = await fetchData("Ssc", "sendSensorsStatus", "{}")
        response = JSON.parse(response)
        console.log(response)
        let arrayInputs = {
            "ciclo": document.getElementById("ciclo"),
            "humidityThreshold": document.getElementById("hr"),
            "temperatureThreshold": document.getElementById("temperature"),
            "uncoverCoffee": document.getElementById("uncoverCoffee"),
            "coverCoffee": document.getElementById("coverCoffee"),
        }        
        let i = 0;
        for (const e in arrayInputs) {
            console.log(e)
            for (let index = 0; index < response.length; index++) {
                if (response[index].sensorName == e) {
                    arrayInputs[e].innerText = response[index].state
                }
            }
            console.log(i)
            i++
        }
    } catch (error) { console.error(error) }
}
getData()

const modifyVariables = async(value, name) => {
    try {
        const valueToChange = document.getElementById(value).value
        const parameters = {
            0: {
                "sensorName": name,
                "state": valueToChange
            }
        }
        let response = await fetchData("Ssc", "updateSensorsStatus", parameters)
        getData()
        document.getElementById(value).value = ""
    } catch (error) { console.error(error) }
}