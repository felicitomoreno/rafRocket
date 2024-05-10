const fetchData = (module, accion, parameters) => {
    return new Promise((resolve, reject) => {
        const url = "https://rocketust2.000webhostapp.com/"
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

const getData = async () => {
    try {
        let response = await fetchData("rocket", "getSystemvar", "{}")
        response = JSON.parse(response)
        console.log(response)
        let arrayInputs = {
            "ciclotrabajo": document.getElementById("ciclotrabajo"),
            "timeWithoutConnectionPermited": document.getElementById("timeWithoutConnectionPermited"),
            "resetSensors": document.getElementById("resetSensors"),
        }
        console.log(arrayInputs);
        let i = 0;
        for (const e in arrayInputs) {
            console.log(e.toString())
            arrayInputs[e].innerText = response[0][e.toString()]
            console.log(response[0][e.toString()])
        }
    } catch (error) { console.error(error) }
}
getData()

const modifyVariables = async (value, name) => {
    try {
        const valueToChange = document.getElementById(value).value
        const parameters = {
            "field": name,
            "value": valueToChange
        }
        let response = await fetchData("rocket", "setSystemvar", parameters)
        getData()
        document.getElementById(value).value = ""
    } catch (error) { console.error(error) }
}