<?php
require 'config.php';
session_start();

$username = "";
if (isset($_SESSION['username']) && $_SESSION['loggedin'] == true) {
    $username = $_SESSION['username'];
} 

try {
    $sql = "SELECT * FROM foods";
    $stmt = $conn->query($sql);
} catch (Exception $e) {
    error_log($e->getMessage());
    exit('Error fetching foods');
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Foods</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link active" href="#">Order Food</a></li>
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
    
    <div class="container foods-holder">
        <?php while ($row = $stmt->fetch()): ?>
            <div class="card" style="width: 18rem;">
                <img src="<?php echo htmlspecialchars($row['image_path']); ?>" class="card-img-top" alt="<?php echo htmlspecialchars($row['name']); ?>">
                <div class="card-body">
                    <h5 class="card-title"><?php echo htmlspecialchars($row['name']); ?></h5>
                    <p class="card-text"><?php echo htmlspecialchars($row['description']); ?></p>
                    <div class="price" style="margin-bottom: 10px;">Ugx <?php echo htmlspecialchars($row['price']); ?></div>
                    <a href="view_food.php?food_id=<?php echo $row['food_id']; ?>" class="btn btn-primary">Order</a>
                </div>
            </div>
        <?php endwhile; ?>
    </div>
    <div class="footer-space"></div>
    <footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
</body>
</html>
