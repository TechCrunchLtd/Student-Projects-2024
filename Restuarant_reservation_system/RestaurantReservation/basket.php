<?php
require_once 'config.php'; 
session_start();

$username = $foodAddSuccessMsg = $success = "";
if (isset($_SESSION['username']) && $_SESSION['loggedin'] == true) {
    $username = $_SESSION['username'];
} else {
    header("location:login.php");
}

if (isset($_GET['action']) && $_GET['action'] == 'delete' && isset($_GET['cart_id'])) {
    // Delete item from cart
    $stmt = $conn->prepare("DELETE FROM cart WHERE cart_id = ? AND username = ? AND status = 'Pending Payment'");
    $stmt->execute([$_GET['cart_id'], $username]);
    header("Location: basket.php");
    exit;
}

// Fetch cart items for the user
$stmt = $conn->prepare("SELECT cart.cart_id, cart.quantity, foods.name, foods.price, foods.image_path FROM cart JOIN foods ON cart.food_id = foods.food_id WHERE cart.username = ? AND status = 'Pending Payment'");
$stmt->execute([$username]);
$cartItems = $stmt->fetchAll();

$totalAmount = 0;
?>

<!DOCTYPE html>  
<html>
<head>
    <title>My Basket</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
    
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="reservation.php">Make Reservation</a></li>
            <li class="nav-item"><a class="nav-link" href="view_foods.php">Order Food</a></li>
            <?php if ($username): ?>
                <li class="nav-item"><a class="nav-link" href="myreservations.php">My Reservations</a></li>
                <li class="nav-item"><a class="nav-link" href="account.php">Account</a></li>
                <li class="nav-item"><a class="nav-link active" href="#">Basket</a></li>
                <li><a class="nav-link" href="logout.php">Log out</a></li>
            <?php else: ?>
                <li><a class="nav-link" href="login.php">Login</a></li>
                <li><a class="nav-link" href="signup.php">Sign up</a></li>
            <?php endif ?>
        </ul>
    </header>
    <div class="container">
    <p style="font-size: 12px;" class='<?php echo $success ? "text-success" : "text-danger"; ?>'><?php echo $foodAddSuccessMsg; ?></p>
        <?php if(count($cartItems) > 0): ?>
        <table class="table">
            <thead>
                <tr>
                    <th>Item Name</th>
                    <th>Quantity</th>
                    <th>Price</th>
                    <th>Total</th>
                    <th>Action</th>
                    <th>Image</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($cartItems as $item): ?>
                    <tr>
                        <td><?php echo htmlspecialchars($item['name']); ?></td>
                        <td><?php echo $item['quantity']; ?></td>
                        <td>Ugx <?php echo $item['price']; ?></td>
                        <td>Ugx <?php echo $item['quantity'] * $item['price']; ?></td>
                        <td><a href="basket.php?action=delete&cart_id=<?php echo $item['cart_id']; ?>">Delete</a></td>
                        <td><div class="table-img"><img class="img-fluid" src="<?php echo $item['image_path']; ?>" alt=""></div></td>
                    </tr>
                    <?php $totalAmount += $item['quantity'] * $item['price']; ?>
                <?php endforeach; ?>
            </tbody>
            <tfoot>
                <tr>
                    <th>Total Amount:</th>
                    <th>Ugx<?php echo $totalAmount; ?></th>
                    <th></th>
                    <th></th>
                    <th></th>
                </tr>

                <tr>
                    <th>Deposit:</th>
                    <th>Ugx<?php echo $totalAmount/2; ?></th>
                    <th></th>
                    <th></th>
                    <th></th>
                </tr>
            </tfoot>
        </table>
        
        <a class="btn btn-primary" href="reservation.php">CHECK OUT</a>
        <?php else: ?>
        <p>Basket is empty. <a href="view_foods.php">Add foods</a></p>
        <?php endif; ?>
    </div>
    <div class="footer-space"></div>
<footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>  
</body>
</html>
