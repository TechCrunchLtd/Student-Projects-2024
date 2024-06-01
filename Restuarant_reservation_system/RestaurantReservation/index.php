<?php 
$username = "";
session_start();
if (isset($_SESSION['username']) && $_SESSION['loggedin'] == true) {
    $username = $_SESSION['username'];
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Dashboard</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
    <link rel="preconnect" href="https://fonts.googleapis.com">
    <link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
    <link href="https://fonts.googleapis.com/css2?family=Madimi+One&display=swap" rel="stylesheet">
    <link href="https://fonts.googleapis.com/css2?family=Madimi+One&family=Satisfy&display=swap" rel="stylesheet">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link active" href="#">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link" href="view_foods.php">Order Food</a></li>
            <?php if ($username): ?>
                <li class="nav-item"><a class="nav-link" href="myreservations.php">My Reservations</a></li>
                <li class="nav-item"><a class="nav-link" href="account.php">Account</a></li>
                <li class="nav-item"><a class="nav-link" href="basket.php">Basket</a></li>
                <li><a class="nav-link" href="logout.php">Log out</a></li>
            <?php else: ?>
                <li><a class="nav-link" href="login.php">Login</a></li>
                <li><a class="nav-link" href="signup.php">Sign up</a></li>
            <?php endif ?>
        </ul>
    </header>
    <div class="header">
        <div class="t1">
            Restaurant Table Reservations System
        </div>

        <div class="t2">
            Welcome
        </div>

        <div class="t3">
        We're thrilled to have you here. Whether you're planning a special occasion or simply craving a delicious meal, our system is here to make your dining experience seamless and memorable. Reserve your table with ease and enjoy a memorable dining experience with us.
        </div>
        <a href="view_foods.php">
            <span class="c1">M</span>
            <span class="c2">A</span>
            <span class="c3">K</span>
            <span class="c4">E</span>
            <span class="c5"></span>
            <span class="c6">R</span>
            <span class="c7">E</span>
            <span class="c8">S</span>
            <span class="c9">E</span>
            <span class="c10">R</span>
            <span class="c11">V</span>
            <span class="c12">A</span>
            <span class="c13">T</span>
            <span class="c14">I</span>
            <span class="c15">O</span>
            <span class="c16">N</span>
        </a>
    </div>
    <div style="background-color: green; padding-top:40px; ">
        <div class="container">
            <div class="intro">
                <div>
                <img src="./IMG/BG/backgroundtable.jpeg" alt="Restaurant Interior">
                </div>
                <div>
                    <div class="para">
                        <p>Indulge in culinary excellence at Gourmet Gatherings, where every dish is crafted with passion and served with a side of hospitality.</p>
                    </div>
                </div>
            </div>
        </div>
    </div><br><br>
    <div>
        <div class="container">
            <div class="menu">
                <img src="./IMG/pizza.jpg" alt="Dessert">
                <img src="./IMG/chicken.jpg" alt="Salad">
                <img src="./IMG/steak.jpg" alt="Steak">
            </div>
        </div>
    </div>

    <div class="footer-space"></div>

    <footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
  
</footer>
</body>
</html>