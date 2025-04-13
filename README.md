# 📘 Sharing App - Backend

👉 To go to the frontend: "" *[(to be filled later)](https://github.com/1997alon/SharingFront)*

---

## ❓ What is this project?

**Sharing App** is a collaborative platform inspired by Wikipedia, but with enhanced control and security:

- Users can **create unique topics**, **upload content**, and **edit** only topics they're authorized to.
- Topics are **publicly viewable**, but **editing is permission-based**.
- The platform includes a built-in **authorization system** and **messaging system**.
- Only registered users can interact with the system and content.

---

## 🧠 Technologies Used

### 🔙 Backend

- **Language:** C++
- **Libraries (installed locally):**
  - [`Boost`](https://www.boost.org/) – `C:\boost_1_87_0`
  - [`MySQL Connector/C++`](https://dev.mysql.com/downloads/connector/cpp/) – `C:\mysql-connector-c++`
  - [`jsoncpp`](https://github.com/open-source-parsers/jsoncpp) – `C:\Users\bardi\Downloads\jsoncpp-master\jsoncpp-master`
- **Architecture:**
  - Asynchronous server built with a **thread pool**
  - Structured using **Object-Oriented Programming (OOP)** with a focus on **polymorphism**
  - Communication between the client and server is handled via **TCP sockets** for secure and reliable messaging
- **Security:**
  - All communication is **encoded and decoded in ASCII**
  - **Passwords are stored as hashes** in the database (no plain-text)
  - **TCP-based communication** helps prevent common web-based vulnerabilities

### 🖥️ Frontend

- **Language:** Python
- **GUI Library:** *(e.g., Tkinter, PyQt — to be added)*
- Provides a user-friendly interface to:
  - Register / Log in
  - View and search topics
  - Edit content (if authorized)
  - Send and receive messages

### 🗄️ Database

- **Engine:** MySQL
- **Tables:**
  - `users` – Stores user credentials and hashed passwords
  - `topics` – Stores topic titles and their content
  - `messages` – Manages messages between users
  - `authorization` – Controls user permissions for editing topics

---

## ✨ Features

- 🧾 Unique topic creation with global visibility
- ✍️ Permission-based editing system
- 👤 User registration and secure authentication
- 🔐 Hashed passwords and ASCII message encoding
- 📩 Internal user messaging system:
  - Users can **request authorization** to edit a topic
  - Topic owners can **approve or deny the request** and reply with a message
- 🌐 Secure TCP socket communication
- 🧵 Multithreaded backend with thread pool
- 🧠 Clean OOP design using polymorphism
- 🔄 Clear separation between frontend and backend

---

## 🛠️ Build Instructions

To build the C++ backend, make sure the following are configured in your project settings or build system:

- **Include directories:**
  - `C:\boost_1_87_0`
  - `C:\mysql-connector-c++\include`
  - `C:\Users\bardi\Downloads\jsoncpp-master\jsoncpp-master\include`

- **Link libraries (as needed):**
  - Boost system and thread libs
  - MySQL Connector/C++ libraries
  - jsoncpp static/dynamic library

Make sure your project links against any `.lib` or `.dll` files provided by MySQL and jsoncpp.

---

Let me know if you want:

- A matching **frontend README**
- A simple **CMakeLists.txt** or Visual Studio setup guide
- **GitHub project badges**
- Or anything else to make your repo shine ✨
