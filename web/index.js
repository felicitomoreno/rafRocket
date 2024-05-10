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

const bringData = async () => {
    try {
        let response = await fetchData("rocket", "getLastData", "{}")
        response = JSON.parse(response)
        console.log(response)
        let i = 0
        const rowTable = document.getElementById("row_table")
        rowTable.innerHTML = ""
        for (const e of response) {
            i++
            rowTable.innerHTML += `
            <!--<tr >
                <th scope="row">${i}</th>
                <td>${e.temperature}</td>
                <td>${e.humidity}</td>
                <td>${e.date}</td>
            </tr>-->


            <tr>
                <th scope="row">${i}</th>
                <td scope="col">${e.rot_x}</td>
                <td scope="col">${e.rot_y}</td>
                <td scope="col">${e.rot_z}</td>
                <td scope="col">${e.acc_x}</td>
                <td scope="col">${e.acc_y}</td>
                <td scope="col">${e.acc_z}</td>
                <td scope="col">${e.vel_x}</td>
                <td scope="col">${e.vel_y}</td>
                <td scope="col">${e.vel_z}</td>    
                <td scope="col">${e.pos_x}</td>
                <td scope="col">${e.pos_y}</td>
                <td scope="col">${e.pos_z}</td>
                <td scope="col">${formatDate(e.createdAt)}</td>
            </tr>
            `
        }
        getLastConnection();
    } catch (error) { console.error(error) }
}

const formatDate = (DateResponse) => {
    let newDate = new Date(DateResponse)
    newDate.setHours(newDate.getHours() - 5)
    return newDate.toLocaleString()
}


const getLastConnection = async () => {
    try {
        let response = await fetchData("rocket", "getlastConnection", "{}")
        response = JSON.parse(response)
        console.log(response)
        let i = 0
        const rowTable = document.getElementById("lastConnection")
        rowTable.innerHTML = ""
        for (const e of response) {
            i++
            rowTable.innerHTML += `
                ${formatDate(e.modifiedAt)}
            `
        }
    } catch (error) { console.error(error) }
}

bringData()
