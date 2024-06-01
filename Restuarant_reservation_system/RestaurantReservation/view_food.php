<?php
require 'config.php'; // Include your database connection
session_start();
// Check if a user is logged in and get their username. 
if (!isset($_SESSION['username'])) {
    // Redirect to login page
    header("location:login.php");
}
$username = $_SESSION['username'];
$success = false;

if (isset($_GET['food_id']))
    $foodId = $_GET['food_id']; // Get the food ID from the query parameter
else
    $foodId = "";
$foodAddSuccessMsg = "";

if ($_SERVER["REQUEST_METHOD"] == "POST") {
    $username = "";
    if (isset($_SESSION['username']) && $_SESSION['loggedin'] == true) {
        $username = $_SESSION['username'];
    } else {
        header("location:login.php");
    }
    $quantity = trim($_POST["quantity"]);
    $foodId = trim($_POST["food_id"]);

    // check if item already in cart
    $sql = "SELECT * FROM cart WHERE food_id = ? AND status = 'Pending Payment' AND username = ?";
    $stmt = $conn->prepare($sql);
    $stmt->execute([$foodId, $username]);
    $stmt->execute();

    if ($stmt->rowCount() > 0) {
        $success = false;
        $foodAddSuccessMsg = "Item already added to basket. Go to basket to view";
        // header("location:view_food.php?food_id=".$foodId);
    } else {
        // add to basket
        $sql = "INSERT INTO cart (username, food_id, quantity, status) VALUES (?, ?, ?, ?)";
        $stmt = $conn->prepare($sql);
        if ($stmt->execute([$username, $foodId, $quantity, "Pending Payment"])) {
            $success = true;
            $foodAddSuccessMsg = "Item added to basket. Redirecting to basket page.";
            ob_start();
            header("refresh: 3; url=basket.php");
            ob_end_flush();
        } else {
            $success = false;
            $foodAddSuccessMsg = "Error adding item to cart.";
        }
    }
} else if (!isset($_GET['food_id'])) {
    header("location:view_foods.php");
}


// Fetch food details
$stmt = $conn->prepare("SELECT * FROM foods WHERE food_id = ?");
$stmt->execute([$foodId]);
$food = $stmt->fetch();

// Fetch reviews for the food
// $reviewsStmt = $conn->prepare("SELECT * FROM reviews WHERE food_id = ? ORDER BY created_at DESC");
// $reviewsStmt->execute([$foodId]);
// $reviews = $reviewsStmt->fetchAll();
?>

<!DOCTYPE html>
<html>
<head>
    <title><?php echo htmlspecialchars($food['name']); ?></title>
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
    <div class="container">
        <div class="text-secondary" style="font-size: 25px;">Order Food</div>
        <p style="font-size: 12px;" class='<?php echo $success ? "text-success" : "text-danger"; ?>'><?php echo $foodAddSuccessMsg; ?></p>

        <div class="view_food_holder">
            <div>
                <img class="img-fluid" src="<?php echo htmlspecialchars($food['image_path']); ?>" alt="">
            </div>
            <div style="padding: 20px;">
                <h5 class="card-title"><?php echo htmlspecialchars($food['name']); ?></h5>
                <p class="text-secondary"><?php echo htmlspecialchars($food['description']); ?></p>
                <div class="price">Ugx <?php echo htmlspecialchars($food['price']); ?></div>
            
                <!-- Quantity Adjustment -->
                <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
                    <label class="text-secondary">Quantity </label>
                    <input class="form-control" type="number" id="quantity" name="quantity" value="1" min="1" style="width: 60px;">
                    <input type="text" id="food_id" name="food_id" value="<?php echo $foodId ?>" hidden><br>
                    <br>
                    <input class="btn btn-primary" type="submit" value="Add to Basket">
                </form>

                
            </div>

        </div>
        <!-- Reviews Section -->
        <!-- <div class="text-secondary" style="font-size: 25px;">Reviews</div> -->
        <!-- <?php // foreach ($reviews as $review): ?> -->
            <!-- <div> -->
                <!-- <p>Rating: <?php // echo $review['rating']; ?>/5</p>
                <p>Comment: <?php // echo htmlspecialchars($review['comment']); ?></p>
                <p>Date: <?php // echo $review['created_at']; ?></p> -->
            <!-- </div> -->
        <!-- <?php // endforeach; ?> -->
    </div>
    <div class="footer-space"></div>
<footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
</body>
</html>
