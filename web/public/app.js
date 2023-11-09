import { initializeApp } from "https://www.gstatic.com/firebasejs/9.1.1/firebase-app.js"
import { getFirestore } from "https://www.gstatic.com/firebasejs/9.1.1/firebase-firestore.js"


const firebaseConfig = {
  apiKey: "AIzaSyA0g3v93bv8A6aWR1ND2C5dcRxz2aRNRXg",
  authDomain: "elsysgk.firebaseapp.com",
  projectId: "elsysgk",
  storageBucket: "elsysgk.appspot.com",
  messagingSenderId: "567528681392",
  appId: "1:567528681392:web:1e46fe2f8bad579c12f075"
}

const app = initializeApp(firebaseConfig)
const db = getFirestore(app)

export { db }