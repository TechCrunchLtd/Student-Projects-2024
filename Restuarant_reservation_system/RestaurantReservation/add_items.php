<?php
require 'config.php'; // Include the database

// Initialize variables for table form
$tableCapacityError = $tableStatusError = $tableNameError = $tableLocationError = "";
$tableCapacity = $tableStatus = $tableName = $tableLocation = "";

// Initialize variables for food form
$foodNameError = $foodDescriptionError = $foodPriceError = $foodImagePathError = "";
$foodName = $foodDescription = $foodPrice = $foodImagePath = "";

// clear messages
$foodAddsuccess = $tableAddsuccess = false;
$foodAddSuccessMsg = $tableAddSuccessMsg = "";

// Check if the table form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["submit_table"])) {
    // Validate table form inputs
    $tableCapacity = trim($_POST["capacity"]);
    $tableStatus = "Available";
    $tableName = trim($_POST["table_name"]);
    $tableLocation = trim($_POST["location"]);

    // Perform your validation checks here (e.g., empty check, numeric check for capacity)
    // For simplicity, I'm only checking if they are empty
    $isValid = true;
    if (empty($tableCapacity)) {
        $tableCapacityError = "Capacity is required.";
        $isValid = false;
    }
    if (empty($tableName)) {
        $tableNameError = "Table name is required.";
        $isValid = false;
    } else {
        // check if table already exists
        $sql = "SELECT table_name FROM tables WHERE table_name = ?";
        $stmt = $conn->prepare($sql);
        $stmt->execute([$tableName]);
        if ($stmt->rowCount() > 0) {
            $tableNameError = "Table with name already exists.";
        }
    }
    if (empty($tableLocation)) {
        $tableLocationError = "Location is required.";
        $isValid = false;
    }

    // If validation passes, insert into database
    if ($isValid) {
        $sql = "INSERT INTO tables (capacity, status, table_name, location) VALUES (?, ?, ?, ?)";
        $stmt = $conn->prepare($sql);
        if ($stmt->execute([$tableCapacity, $tableStatus, $tableName, $tableLocation])) {
            $tableAddsuccess = true;
            $tableAddSuccessMsg = "Table added successfully.";
            $tableCapacity = $tableStatus = $tableName = $tableLocation = "";
        } else {
            $tableAddsuccess = false;
            $tableAddSuccessMsg = "Error adding table.";
        }
    }
}

// Check if the food form is submitted
if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["submit_food"])) {
    // Validate food form inputs
    $foodName = trim($_POST["name"]);
    $foodDescription = trim($_POST["description"]);
    $foodPrice = trim($_POST["price"]);
    $foodImagePath = trim($_POST["image_path"]);

    // Perform your validation checks here
    $isValid = true;
    if (empty($foodName)) {
        $foodNameError = "Name is required.";
        $isValid = false;
    } else {
        // check if food already exists
        $sql = "SELECT name FROM foods WHERE name = ?";
        $stmt = $conn->prepare($sql);
        $stmt->execute([$foodName]);
        if ($stmt->rowCount() > 0) {
            $foodNameError = "Food with name already exists.";
        }
    }
    if (empty($foodDescription)) {
        $foodDescriptionError = "Description is required.";
        $isValid = false;
    }
    if (empty($foodPrice)) {
        $foodPriceError = "Price is required.";
        $isValid = false;
    }
    if (empty($foodImagePath)) {
        $foodImagePathError = "Image path is required.";
        $isValid = false;
    } elseif (empty(trim($_POST["hiddenImageUrl"]))) {
        $foodImagePathError = "Invalid image url.";
        $isValid = false;
    }

    // If validation passes, insert into database
    if ($isValid) {
        $sql = "INSERT INTO foods (name, description, price, image_path) VALUES (?, ?, ?, ?)";
        $stmt = $conn->prepare($sql);
        if ($stmt->execute([$foodName, $foodDescription, $foodPrice, $foodImagePath])) {
            $foodName = $foodDescription = $foodPrice = $foodImagePath = "";
            $foodAddsuccess = true;
            $foodAddSuccessMsg = "Food added successfully.";
        } else {
            $foodAddsuccess = false;
            $foodAddSuccessMsg = "Error adding food.";
        }
    }
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Add Tables and Foods</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="admin_dash.php">Admin Dashboard</a></li>
            <li class="nav-item"><a class="nav-link" href="#">Add Items</a></li>
            <li class="nav-item"><a class="nav-link" href="items.php">Tables & Food</a></li>
            <li class="nav-item"><a class="nav-link" href="users.php">Users</a></li>
            <li class="nav-item"><a class="nav-link" href="transactions.php">Transactions</a></li>
            <li class="nav-item"><a class="nav-link" href="logout.php">Log out</a></li>
        </ul>
    </header>
    <div class="container add-forms-holder">
        <div>
            <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
                <div class="text-secondary" style="font-size: 25px;">Add Table</div>
                <p style="font-size: 12px;" class='<?php echo $tableAddsuccess ? "text-success" : "text-danger"; ?>'><?php echo $tableAddSuccessMsg; ?></p>
                
                <label>Capacity</label> <input class="form-control" type="text" name="capacity" value="<?php echo htmlspecialchars($tableCapacity);?>">
                <span class="error"><?php echo $tableCapacityError;?></span><br>
                <label>Table Name</label> <input class="form-control" type="text" name="table_name" value="<?php echo htmlspecialchars($tableName);?>">
                <span class="error"><?php echo $tableNameError;?></span><br>
                <label>Location</label> <input class="form-control" type="text" name="location" value="<?php echo htmlspecialchars($tableLocation);?>">
                <span class="error"><?php echo $tableLocationError;?></span><br>
                <input class="btn btn-primary" type="submit" name="submit_table" value="Add Table">
            </form>
        </div>
        <div>
            <form method="post" action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]);?>">
                <div class="text-secondary" style="font-size: 25px;">Add Food</div>
                <p style="font-size: 12px;" class='<?php echo $foodAddsuccess ? "text-success" : "text-danger"; ?>'><?php echo $foodAddSuccessMsg; ?></p>

                <label>Name</label> <input class="form-control" type="text" name="name" value="<?php echo htmlspecialchars($foodName);?>">
                <span class="error"><?php echo $foodNameError;?></span><br>
                <label>Description</label> <input class="form-control" type="text" name="description" value="<?php echo htmlspecialchars($foodDescription);?>">
                <span class="error"><?php echo $foodDescriptionError;?></span><br>
                <label>Price</label> <input class="form-control" type="text" name="price" value="<?php echo htmlspecialchars($foodPrice);?>">
                <span class="error"><?php echo $foodPriceError;?></span><br>
                <label>Image Path</label> <input id="imageUrlInput" class="form-control" type="text" name="image_path" value="">
                <span class="error"><?php echo $foodImagePathError;?></span><br>
                <br>
                <input type="hidden" id="hiddenImageUrl" name="hiddenImageUrl">
                <div class="add-items-img">
                <img class="img-fluid" id="imagePreview" src="" alt="Preview"><br>
                </div>
                <input class="btn btn-primary" type="submit" name="submit_food" value="Add Food">
            </form>
        </div>
    </div>
    <div class="footer-space"></div>
    <footer class="footer mt-auto py-3 bg-light">
  <div class="container">
    <span class="text-muted">&copy Restaurant Reservations</span>
  </div>
</footer>
    <script>
        // Function to check if a URL is a valid image URL
        function isValidImageUrl(url) {
            var img = new Image();
            img.src = url;
            img.onload = function() {
                console.log("Valid image URL");
                document.getElementById("hiddenImageUrl").value = url;
                document.getElementById("imagePreview").src = url;
            };

            img.onerror = function() {
                console.log("Invalid image URL");
                document.getElementById("hiddenImageUrl").value = "";
                document.getElementById("imagePreview").src = "";
            };
        }

        // Get input element
        var imageUrlInput = document.getElementById("imageUrlInput");

        // Add event listener for input change
        imageUrlInput.addEventListener("change", function() {
            var imageUrl = imageUrlInput.value;
            isValidImageUrl(imageUrl);
        });
    </script>
</body>
</html>
