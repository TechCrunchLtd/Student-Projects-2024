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
        <div style="font-size: 30px;">Welcome <?php echo $username; ?> to Restaurant Reservations</div>
    </div>
    <div style="background-color:#0d6efd; padding-top:20px; ">
        <div class="container">
            <div class="intro" style="padding-bottom:20px;">
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
    </div>
    <div>
        <div class="container">
            <div class="menu">
                <img src="./IMG/pizza.jpg" alt="Dessert">
                <img src="./IMG/chicken.jpg" alt="Salad">
                <img src="./IMG/steak.jpg" alt="Steak">
            </div>
        </div>
    </div>
    <div style="background-color:#0d6efd; padding-top:20px; ">
        <div class="container">
            <div class="reservation">
                <img src="./IMG/meeting.jpg" alt="Business Meeting">
                <p>Elevate your meetings with our elegant and spacious dining areas, perfect for productive discussions and networking opportunities.</p>
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