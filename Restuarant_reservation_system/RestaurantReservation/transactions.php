<?php
require_once 'config.php'; 

$username = "";
session_start();
// redirect if user is not admin
if ($_SESSION['role'] !== "admin") {
    header("location:index.php");
    exit();
}

// Fetch cart items for the user
$stmt = $conn->prepare("SELECT * FROM transactions");
$stmt->execute([]);
$cartItems = $stmt->fetchAll();

$totalAmount = 0;
?>

<!DOCTYPE html>  
<html>
<head>
    <title>Transactions</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
    
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="admin_dash.php">Admin Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="add_items.php">Add Items</a></li>
            <li class="nav-item"><a class="nav-link" href="items.php">Tables & Food</a></li>
            <li class="nav-item"><a class="nav-link" href="users.php">Users</a></li>
            <li class="nav-item"><a class="nav-link" href="#">Transactions</a></li>
            <li class="nav-item"><a class="nav-link" href="logout.php">Log out</a></li>
        </ul>
    </header>
    <div class="container">
        <?php if(count($cartItems) > 0): ?>
        <table class="table">
            <thead>
                <tr>
                    <th>Transaction ID</th>
                    <th>Payer Name</th>
                    <th>Paid Amount</th>
                    <th>Payment Method</th>
                    <th>Date</th>
                </tr>
            </thead>
            <tbody>
                <?php foreach ($cartItems as $item): ?>
                    <tr>
                        <td><?php echo htmlspecialchars($item['transaction_id']); ?></td>
                        <td><?php echo $item['payer_name']; ?></td>
                        <td>Ugx <?php echo $item['paid_amount'] * 100; ?></td>
                        <td><?php echo $item['payment_source']; ?></td>
                        <td><?php echo $item['created']; ?></td>
                         </tr>
                <?php endforeach; ?>
            </tbody>
        </table>
        <?php else: ?>
        <p>No transactions yet.</p>
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
