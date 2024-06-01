<?php
session_start();
require 'config.php'; // include database

$username = $reservationMsg = "";
$success = false;
if (isset($_SESSION['username']) && $_SESSION['loggedin'] == true) {
    $username = $_SESSION['username'];
} else {
    header("location:login.php");
}

$cartItems = [];
try {
    $food_stmt = $conn->prepare("SELECT cart.cart_id, cart.quantity, foods.name, foods.price, foods.image_path FROM cart JOIN foods ON cart.food_id = foods.food_id WHERE cart.username = ? AND status = 'Pending Payment'");
    $food_stmt->execute([$username]);
    $cartItems = $food_stmt->fetchAll();
    $totalAmount = 0;
} catch (Exception $e) {
    error_log($e->getMessage());
    exit('Error fetching tables');
}

$disabled = false;
if (count($cartItems) == 0) {
    $reservationMsg = "No food in basket. Add food in basket before making resrvation.";
    $success = false;
    $disabled = true;
}

if ($_SERVER["REQUEST_METHOD"] == "POST" && isset($_POST["make_reservation"])) {
    $stmt = $conn->prepare("SELECT cart.cart_id, cart.quantity, foods.name, foods.price, foods.image_path FROM cart JOIN foods ON cart.food_id = foods.food_id WHERE cart.username = ? AND status = 'Pending Payment'");
    $stmt->execute([$username]);
    $cartItems = $stmt->fetchAll();
    if (count($cartItems) != 0){
        $success = false;
        $reservationMsg = "Payment not received. Complete payment first.";
    } else {
        $tableId = $_POST['table_id'];
        $reservationDate = $_POST['reservation_date'];
        $reservationTime = $_POST['reservation_time'];
        $duration = $_POST['duration'];
        $status = 'Pending';

        // Convert reservation time to DateTime objects for easier manipulation
        $startDateTime = new DateTime("$reservationDate $reservationTime");
        $endDateTime = clone $startDateTime;
        $endDateTime->modify("+{$duration} hour");

        // Ensure reservation start time is at least 10 minutes in the future
        $currentDateTime = new DateTime();

        if ($startDateTime <= $currentDateTime || $currentDateTime->diff($startDateTime)->i < 10 && $currentDateTime->diff($startDateTime)->h == 0 && $currentDateTime->diff($startDateTime)->days == 0) {
            $reservationError = "Reservation must be made at least 10 minutes in advance. Check date and time";
        } else {
            $startDateTime = new DateTime("$reservationDate $reservationTime");
            $endDateTime = clone $startDateTime;
            $endDateTime->modify("+{$duration} hour");
            // Convert back to strings for SQL query
            $startTime = $startDateTime->format('Y-m-d H:i:s');
            $endTime = $endDateTime->format('Y-m-d H:i:s');

            // SQL query to check for overlapping reservations
            $sql = "SELECT * FROM reservations WHERE reservation_date = ? AND table_id = ? AND (
                (TIME(reservation_time) < TIME(?) AND TIME(DATE_ADD(reservation_time, INTERVAL duration HOUR)) > TIME(?))
                OR 
                (TIME(reservation_time) < TIME(?) AND TIME(DATE_ADD(reservation_time, INTERVAL duration HOUR)) > TIME(?))
            )";

            $stmt = $conn->prepare($sql);
            $stmt->execute([$reservationDate, $tableId, $reservationTime, $reservationTime, $endTime, $endTime]);
            $stmt->execute();
            $results = $stmt->fetchAll();

            // Check if any reservations overlap the new reservation time
            if (count($results) > 0) {
                $reservationError = "There is already a reservation within the specified time range. Please choose a different time or table.";
            } else {
                // Insert reservation into database
                $sql = "INSERT INTO reservations (reservation_date, reservation_time, duration, status, username, table_id) VALUES (?, ?, ?, ?, ?, ?)";
                try {
                    $stmt = $conn->prepare($sql);
                    $stmt->execute([$reservationDate, $reservationTime, $duration, $status, $username, $tableId]);
                    $lastInsertId = $conn -> lastInsertId();
                    $updateSql = "UPDATE cart SET reservation_id = :rid where status = 'DEPOSIT PAID' AND reservation_id IS NULL";
                    $updateStmt = $conn->prepare($updateSql);
                    $updateStmt->bindParam(':rid', $lastInsertId);
                    $updateStmt->execute();
                    $success = true;
                    $reservationMsg = "Payment and Reservation successful. Redirection to my reservations page.";
                    ob_start();
                    header("refresh: 3; url=myreservations.php");
                    ob_end_flush();
                } catch (Exception $e) {
                    $reservationMsg = $e->getMessage();
                }
            }

        } 
    }   
}

// Fetch available tables
try {
    $sql = "SELECT * FROM tables WHERE status = 'Available'";
    $stmt = $conn->query($sql);
} catch (Exception $e) {
    error_log($e->getMessage());
    exit('Error fetching tables');
}
?>

<!DOCTYPE html>
<html>
<head>
    <title>Select Table</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3" crossorigin="anonymous">
    <link rel="stylesheet" href="./CSS/styles.css">
    <script src="https://www.paypal.com/sdk/js?client-id=<?php echo PAYPAL_SANDBOX?PAYPAL_SANDBOX_CLIENT_ID:PAYPAL_PROD_CLIENT_ID; ?>&currency=<?php echo $currency; ?>"></script>
</head>
<body>
    <header>
        <ul class="nav justify-content-center">
            <li class="nav-item"><a class="nav-link" href="index.php">Dashboard</a></li>
            <li class="nav-item"><a class="nav-link active" href="#">Make Reservation</a></li>
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
    <h2></h2>
    <div class="container flexed-div">
        <div>
        <?php if (count($cartItems) > 0): ?>
            <div class="text-secondary" style="font-size: 25px;">Foods ordered</div>
            <table id="foods_table" class="table">
                <thead>
                    <th>Item name</th>
                    <th>Quantity</th>
                    <th>Price</th>
                    <th>Total</th>
                    <th>Image</th>
                </thead>
                <tbody>
                    <?php foreach ($cartItems as $item): ?>
                        <tr>
                            <td><?php echo htmlspecialchars($item['name']); ?></td>
                            <td><?php echo $item['quantity']; ?></td>
                            <td>Ugx <?php echo $item['price']; ?></td>
                            <td>Ugx <?php echo $item['quantity'] * $item['price']; ?></td>
                            <td><div class="table-img"><img class="img-fluid" src="<?php echo $item['image_path']; ?>" alt=""></div></td>
                        </tr>
                        <?php $totalAmount += $item['quantity'] * $item['price']; ?>
                    <?php endforeach; ?>
                </tbody>
                <tfoot>
            </tfoot>
            </table>

            <?php $itemPrice = $totalAmount/200; ?>
            <div class="panel">
                <div class="overlay hidden"><div class="overlay-content"></div></div>

                <div class="panel-heading">
                    <h3 class="panel-title">Charge deposit <?php echo 'KES '.$totalAmount/2; ?> with PayPal</h3>
                    
                </div>
                <div class="panel-body">
                    <!-- Display status message -->
                    <div id="paymentResponse" class="hidden"></div>
                    
                    <!-- Set up a container element for the button -->
                    <div id="paypal-button-container"></div>
                </div>
            </div>
            <?php endif; ?>
        </div>

        <div>
        <form action="<?php echo htmlspecialchars($_SERVER["PHP_SELF"]); ?>" method="post">
            <div class="text-secondary" style="font-size: 25px;">Select a Table for Your Reservation</div>
            <p style="font-size: 12px;" class='<?php echo $success ? "text-success" : "text-danger"; ?>'><?php echo $reservationMsg; ?></p>
            <label for="table_id">Table:</label>
            <select class="form-select" name="table_id" id="table_id" required>
                <?php while ($row = $stmt->fetch()): ?>
                    <option value="<?php echo $row['table_id']; ?>"><?php echo htmlspecialchars($row['table_name']) . " - Location: " . htmlspecialchars($row['location']). " - Capacity: " .htmlspecialchars($row['capacity']); ?></option>
                <?php endwhile; ?>
            </select><br>
            
            <label for="reservation_date">Date:</label>
            <input class="form-control" type="date" id="reservation_date" name="reservation_date" required><br>
            
            <label for="reservation_time">Time:</label>
            <input class="form-control" type="time" id="reservation_time" name="reservation_time" required><br>
            
            <label for="duration">Duration (hours):</label>
            <input class="form-control" type="number" id="duration" name="duration" value="1" min="1" max="5" required><br>

            
            <input class="btn btn-primary" type="submit" name="make_reservation" value="Make Reservation" <?php echo $disabled ? "disabled" : ""; ?> >
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
paypal.Buttons({
    // Sets up the transaction when a payment button is clicked
    createOrder: (data, actions) => {
        return actions.order.create({
            "purchase_units": [{
                "custom_id": "<?php echo $itemNumber; ?>",
                "description": "<?php echo $itemName; ?>",
                "amount": {
                    "currency_code": "<?php echo $currency; ?>",
                    "value": <?php echo $itemPrice; ?>,
                    "breakdown": {
                        "item_total": {
                            "currency_code": "<?php echo $currency; ?>",
                            "value": <?php echo $itemPrice; ?>
                        }
                    }
                },
                "items": [
                    {
                        "name": "<?php echo $itemName; ?>",
                        "description": "<?php echo $itemName; ?>",
                        "unit_amount": {
                            "currency_code": "<?php echo $currency; ?>",
                            "value": <?php echo $itemPrice; ?>
                        },
                        "quantity": "1",
                        "category": "DIGITAL_GOODS"
                    },
                ]
            }]
        });
    },
    // Finalize the transaction after payer approval
    onApprove: (data, actions) => {
        return actions.order.capture().then(function(orderData) {
            setProcessing(true);

            var postData = {paypal_order_check: 1, order_id: orderData.id};
            fetch('paypal_checkout_validate.php', {
                method: 'POST',
                headers: {'Accept': 'application/json'},
                body: encodeFormData(postData)
            })
            .then((response) => response.json())
            .then((result) => {
                if(result.status == 1){
                    document.getElementById('checkout_ref').value = result.ref_id;
                }else{
                    const messageContainer = document.querySelector("#paymentResponse");
                    messageContainer.classList.remove("hidden");
                    messageContainer.textContent = result.msg;
                    
                    setTimeout(function () {
                        messageContainer.classList.add("hidden");
                        messageText.textContent = "";
                    }, 5000);
                }
                setProcessing(false);
            })
            .catch(error => console.log(error));
        });
    }
}).render('#paypal-button-container');

const encodeFormData = (data) => {
  var form_data = new FormData();

  for ( var key in data ) {
    form_data.append(key, data[key]);
  }
  return form_data;   
}

// Show a loader on payment form processing
const setProcessing = (isProcessing) => {
    if (isProcessing) {
        document.querySelector(".overlay").classList.remove("hidden");
    } else {
        document.querySelector(".overlay").classList.add("hidden");
    }
}    
</script>
</body>
</html>