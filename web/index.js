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

const bringData = async() => {
    try {
        let response = await fetchData("Ssc", "sendTemperatureAndHumidityLogs", "{}")
        response = JSON.parse(response)
        console.log(response)
        let i = 0
        const rowTable = document.getElementById("row_table")
        rowTable.innerHTML = ""
        for (const e of response) {
            i++
            rowTable.innerHTML += `
            <tr >
                <th scope="row">${i}</th>
                <td>${e.temperature}</td>
                <td>${e.humidity}</td>
                <td>${e.date}</td>
            </tr>
            `
        }
    } catch (error) { console.error(error) }
}
bringData()
