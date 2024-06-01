<?php 
// Include the configuration file 
require_once 'config.php'; 
ini_set('display_errors', 0);
error_reporting(E_ALL);
ini_set('log_errors', 1);
ini_set('error_log', 'php-error.log');
 
// Include the PayPal API library 
require_once 'PaypalCheckout.class.php'; 
$paypal = new PaypalCheckout; 

session_start();

$username = $foodAddSuccessMsg = $success = "";
if (isset($_SESSION['username']) && $_SESSION['loggedin'] == true) {
    $username = $_SESSION['username'];
} else {
    header("location:login.php");
}
 
$response = array('status' => 0, 'msg' => 'Transaction Failed!'); 
if(!empty($_POST['paypal_order_check']) && !empty($_POST['order_id'])){ 
    // Validate and get order details with PayPal API 
    try {  
        $order = $paypal->validate($_POST['order_id']); 
    } catch(Exception $e) {  
        $api_error = $e->getMessage();  
    } 
     
    if(!empty($order)){ 
        $order_id = $order['id']; 
        $intent = $order['intent']; 
        $order_status = $order['status']; 
        $order_time = date("Y-m-d H:i:s", strtotime($order['create_time'])); 
 
        if(!empty($order['purchase_units'][0])){ 
            $purchase_unit = $order['purchase_units'][0]; 
 
            $item_number = $purchase_unit['custom_id']; 
            $item_name = $purchase_unit['description']; 
             
            if(!empty($purchase_unit['amount'])){ 
                $currency_code = $purchase_unit['amount']['currency_code']; 
                $amount_value = $purchase_unit['amount']['value']; 
            } 
 
            if(!empty($purchase_unit['payments']['captures'][0])){ 
                $payment_capture = $purchase_unit['payments']['captures'][0]; 
                $transaction_id = $payment_capture['id']; 
                $payment_status = $payment_capture['status']; 
            } 
 
            if(!empty($purchase_unit['payee'])){ 
                $payee = $purchase_unit['payee']; 
                $payee_email_address = $payee['email_address']; 
                $merchant_id = $payee['merchant_id']; 
            } 
        } 
 
        $payment_source = ''; 
        if(!empty($order['payment_source'])){ 
            foreach($order['payment_source'] as $key=>$value){ 
                $payment_source = $key; 
            } 
        } 
 
        if(!empty($order['payer'])){ 
            $payer = $order['payer']; 
            $payer_id = $payer['payer_id']; 
            $payer_name = $payer['name']; 
            $payer_given_name = !empty($payer_name['given_name'])?$payer_name['given_name']:''; 
            $payer_surname = !empty($payer_name['surname'])?$payer_name['surname']:''; 
            $payer_full_name = trim($payer_given_name.' '.$payer_surname); 
            $payer_full_name = filter_var($payer_full_name, FILTER_SANITIZE_STRING,FILTER_FLAG_STRIP_HIGH); 
 
            $payer_email_address = $payer['email_address']; 
            $payer_address = $payer['address']; 
            $payer_country_code = !empty($payer_address['country_code'])?$payer_address['country_code']:''; 
        } 
 
        if(!empty($order_id) && $order_status == 'COMPLETED'){ 
            // Check if any transaction data is exists already with the same TXN ID 
            $sqlQ = "SELECT id FROM transactions WHERE transaction_id = :transaction_id"; 
            $stmt = $conn->prepare($sqlQ);  
            $stmt->execute([':transaction_id' => $transaction_id]); 
            $existingTransaction = $stmt->fetch(); 
             
            $payment_id = 0; 
            if($existingTransaction){ 
                $payment_id = $existingTransaction['id'];
            } else { 
                // Insert transaction data into the database 
                $sqlQ = "INSERT INTO transactions (item_number, item_name, item_price, item_price_currency, payer_id, payer_name, payer_email, payer_country, merchant_id, merchant_email, order_id, transaction_id, paid_amount, paid_amount_currency, payment_source, payment_status, created) VALUES (:item_number, :item_name, :itemPrice, :currency, :payer_id, :payer_full_name, :payer_email_address, :payer_country_code, :merchant_id, :payee_email_address, :order_id, :transaction_id, :amount_value, :currency_code, :payment_source, :payment_status, NOW())"; 
                $stmt = $conn->prepare($sqlQ); 
                $insert = $stmt->execute([
                    ':item_number' => $item_number, 
                    ':item_name' => $item_name, 
                    ':itemPrice' => $itemPrice, 
                    ':currency' => $currency, 
                    ':payer_id' => $payer_id, 
                    ':payer_full_name' => $payer_full_name, 
                    ':payer_email_address' => $payer_email_address, 
                    ':payer_country_code' => $payer_country_code, 
                    ':merchant_id' => $merchant_id, 
                    ':payee_email_address' => $payee_email_address, 
                    ':order_id' => $order_id, 
                    ':transaction_id' => $transaction_id, 
                    ':amount_value' => $amount_value, 
                    ':currency_code' => $currency_code, 
                    ':payment_source' => $payment_source, 
                    ':payment_status' => $payment_status
                ]); 
                 
                if($insert){ 
                    $payment_id = $conn->lastInsertId(); 
                }
            }

            $updatePaid = "UPDATE cart SET status = 'DEPOSIT PAID' WHERE username = ?";
            $stmt = $conn->prepare($updatePaid);
            $stmt->execute([$username]);
        
            if(!empty($payment_id)){ 
                $ref_id_enc = base64_encode($transaction_id); 
                $response = array('status' => 1, 'msg' => 'Transaction completed!', 'ref_id' => $ref_id_enc); 
            } 
        } 
    }else{ 
        $response['msg'] = $api_error; 
    } 
} 
header('Content-Type: application/json');
echo json_encode($response); 
?>