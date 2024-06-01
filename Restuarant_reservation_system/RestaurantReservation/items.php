<?php 

require 'config.php';

// Function to fetch all tables
function fetchAllTables($conn) {
    $sql = "SELECT * FROM `tables`";
    $stmt = $conn->prepare($sql);
    $stmt->execute();
    $result = $stmt->fetchAll();
    $tables = [];
    foreach ($result as $row) {
        $tables[] = $row;
    }
    return $tables;
}

// Function to fetch all foods
function fetchAllFoods($conn) {
    $sql = "SELECT * FROM `foods`";
    $stmt = $conn->prepare($sql);
    $stmt->execute();
    $result = $stmt->fetchAll();
    $foods = [];
    foreach ($result as $row) {
        $foods[] = $row;
    }
    return $foods;
}

// Handling deletion of tables
if (isset($_POST['delete_table'])) {
    $tableId = $_POST['table_id'];
    $sql = "DELETE FROM `tables` WHERE `table_id` = :i";
    $stmt = $conn->prepare($sql);
    $stmt->bindParam(":i", $tableId);
    $stmt->execute();
    header("location:items.php");
}

// Handling deletion of foods
if (isset($_POST['delete_food'])) {
    $foodId = $_POST['food_id'];
    $sql = "DELETE FROM `foods` WHERE `food_id` = :i";
    $stmt = $conn->prepare($sql);
    $stmt->bindParam(":i", $foodId);
    $stmt->execute();
    header("location:items.php");
}

// Fetching all tables and foods
$tables = fetchAllTables($conn);
$foods = fetchAllFoods($conn);


?>

<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Document</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="admin_dash.php">Admin Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="add_items.php">Add Items</a></li>
            <li class="nav-item"><a class="nav-link" href="#">Tables & Food</a></li>
            <li class="nav-item"><a class="nav-link" href="users.php">Users</a></li>
            <li class="nav-item"><a class="nav-link" href="transactions.php">Transactions</a></li>
            <li class="nav-item"><a class="nav-link" href="logout.php">Log out</a></li>
        </ul>
    </header>

    <div class="container">
        <div class="text-secondary" style="font-size: 25px;">All Tables</div>
        <div class="foods-holder">
        <?php foreach ($tables as $row): ?>
            <div class="card" style="width: 18rem;">
                <div class="card-body">
                    <h5 class="card-title"><?php echo htmlspecialchars($row['table_name']); ?></h5>
                    <p class="card-text">Capacity: <?php echo htmlspecialchars($row['capacity']); ?></p>
                    <div class="price" style="margin-bottom: 10px;">Location: <?php echo htmlspecialchars($row['location']); ?></div>
                    <form method="post">
                    <input type="hidden" name="table_id" value="<?php echo $table['table_id']; ?>">
                    <button class='btn btn-danger' type="submit" name="delete_table">Delete Table</button>
                    </form>
                    
                </div>
            </div>
        <?php endforeach; ?>
        </div>

        <div class="text-secondary" style="font-size: 25px;">All Foods</div>
        <div class="foods-holder">
            <?php foreach ($foods as $row): ?>
                <div class="card" style="width: 18rem;">
                        <img src="<?php echo htmlspecialchars($row['image_path']); ?>" class="card-img-top" alt="<?php echo htmlspecialchars($row['name']); ?>">
                        <div class="card-body">
                            <h5 class="card-title"><?php echo htmlspecialchars($row['name']); ?></h5>
                            <p class="card-text"><?php echo htmlspecialchars($row['description']); ?></p>
                            <div class="price" style="margin-bottom: 10px;">Ugx <?php echo htmlspecialchars($row['price']); ?></div>
                            <form method="post">
                                <input type="hidden" name="food_id" value="<?php echo $table['food_id']; ?>">
                                <button class='btn btn-danger' type="submit" name="delete_food">Delete Food</button>
                            </form>
                        </div>
                    </div>
            <?php endforeach; ?>
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