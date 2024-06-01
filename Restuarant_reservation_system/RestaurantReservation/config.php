<?php

$host = 'localhost'; // or your database host
$dbname = 'restaurantreservationdb';
$username = 'root';
$password = '';

try {
    $conn = new PDO("mysql:host=$host;dbname=$dbname", $username, $password, [
        PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
        PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC,
    ]);
        // Product Details 
    $itemNumber = "DP12345"; 
    $itemName = "Demo Product"; 
    $itemPrice = 75;  
    $currency = "USD"; 
    
    /* PayPal REST API configuration 
    * You can generate API credentials from the PayPal developer panel. 
    * See your keys here: https://developer.paypal.com/dashboard/ 
    */ 
    define('PAYPAL_SANDBOX', TRUE); //TRUE=Sandbox | FALSE=Production 
    define('PAYPAL_SANDBOX_CLIENT_ID', 'AWUOtYPlQ7m63NHZVIKohDPAAVVm-rULX4j5W23yVL6VFjcdbKEM90xz9YJK2uUTfOpyJfwbGpXFZp0q'); 
    define('PAYPAL_SANDBOX_CLIENT_SECRET', 'EK7bKdSajkG4h9Dqjp-GX_SKyY_YrEzTDP9gK3FRnN1d8bTYH08TNAiELjIZ6sHwKcEvCIR81fB9sqa3'); 

} catch (PDOException $e) {
    die("Could not connect to the database $dbname :" . $e->getMessage());
}
?>
