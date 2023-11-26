import * as THREE from "three"
import { collection, getDocs, orderBy } from "firebase/firestore"
import { db } from "./app.js"


const button2D = document.getElementById("button2D")
const button3D = document.getElementById("button3D")
const container2D = document.getElementById("container2D")
const container3D = document.getElementById("container3D")

button2D.onclick = () => {
    container2D.style.display = "block"
    container3D.style.display = "none"
}

button3D.onclick = () => {
    container2D.style.display = "none"
    container3D.style.display = "block"
}

const ctx = document.getElementById('myChart')
new Chart(ctx, {
    type: 'line',
    data: {
        labels: [1, 2, 3, 4, 5, 6, 7],
        datasets: [{
            // label: 'My First Dataset',
            data: [65, 59, 80, 81, 56, 55, 40],
            // borderColor: 'rgb(75, 192, 192)',
            // tension: 0.1
        }]
    },
    options: {
        animation: false,
        transitions: {
            activate: {
                animation: {
                    duration: 0
                }
            }
        }
        // scales: {
        //     y: {
        //         beginAtZero: true
        //     }
        // }
    }
})


const sensorDataList = document.getElementById("sensorDataList")
const sensorData = new Map()


const querySnapshot = await getDocs(collection(db, "sensorData"), orderBy("created_at", "desc"))
querySnapshot.forEach((doc) => {
    sensorData[doc.id] = doc.data().sensors
    const li = document.createElement("li")
    const date = new Date(doc.id*1000)
    li.innerText = date.toLocaleString('no-NO', { timeZone: 'Europe/Oslo' })
    li.setAttribute("id", doc.id)
    li.onclick = () => {
        console.log(sensorData[li.id])
        scene.remove(mesh)
        mesh = createMesh(scene, sensorData[li.id], mesh.rotation.y)
    }
    sensorDataList.appendChild(li)
})


const scene = new THREE.Scene()
const camera = new THREE.PerspectiveCamera(75, 7 / 4, 0.1, 1000)

let mesh = createMesh(scene, [1, 1, 1, 1, 1, 1, 1, 1, 1], 0)

const renderer = new THREE.WebGLRenderer()
renderer.setSize(700, 400)
container3D.appendChild(renderer.domElement)

const pg = new THREE.PlaneGeometry(100, 100)
const pm = new THREE.MeshPhongMaterial({color: 0x0000ff, side: THREE.DoubleSide})
const plane = new THREE.Mesh(pg, pm)
plane.rotation.x = Math.PI / 2
plane.position.y = -0.1
scene.add(plane)

const light = new THREE.PointLight(0xffffff, 100)
light.position.set(6, 10, 2)
scene.add(light)


function createMesh(scene, h, ry) {
    const d = 8
    const points = [
        d, h[0], d,
        0, h[1], d,
        d, h[3], 0,

        0, h[1], d,
        0, h[4], 0,
        d, h[3], 0,

        0, h[1], d,
        -d, h[2], d,
        0, h[4], 0,

        -d, h[2], d,
        0, h[4], 0,
        -d, h[5], 0,

        d, h[3], 0,
        0, h[4], 0,
        d, h[6], -d,

        0, h[4], 0,
        0, h[7], -d,
        d, h[6], -d,

        0, h[4], 0,
        -d, h[5], 0,
        0, h[7], -d,

        -d, h[5], 0,
        -d, h[8], -d,
        0, h[7], -d
    ]

    const bg = new THREE.BufferGeometry()
    const positionAttribute = new THREE.BufferAttribute(new Float32Array(points), 3)
    bg.setAttribute('position', positionAttribute)

    const bm = new THREE.MeshPhongMaterial({ color: 0x00ff00, side: THREE.DoubleSide })
    bg.computeVertexNormals()
    const mesh = new THREE.Mesh(bg, bm)
    mesh.rotation.y = ry

    scene.add(mesh)
    return mesh
}


camera.position.z = 18
camera.position.y = 15
camera.rotation.x -= 2/Math.PI


function animate() {
	requestAnimationFrame(animate)
    mesh.rotation.y += 0.01
	renderer.render(scene, camera)
}

animate()

document.addEventListener("keydown", (event) => {
    console.log(event.key)
    switch(event.key) {
        case "w":
            camera.position.z--
            break
        
        case "s":
            camera.position.z++
            break
        
        case "a":
            camera.position.x--
            break
        
        case "d":
            camera.position.x++
            break
        
        case "q":
            camera.position.y--
            break
        
        case "e":
            camera.position.y++
            break
        
        case "ArrowLeft":
            camera.rotation.y += 1/Math.PI
            break
        
        case "ArrowRight":
            camera.rotation.y -= 1/Math.PI
            break
        
        case "ArrowUp":
            camera.rotation.x -= 1/Math.PI
            break
        
        case "ArrowDown":
            camera.rotation.x += 1/Math.PI
            break
        
    }
})