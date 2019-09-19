<?php


/**
 * Build the collection of email addresses
 * Use the group list and the manually inputted addresses
 * TODO: Need to add records to the database
 */
function pressrelease_collect($group_list, $addresses) {
	// We need some default arrays setup
	$email_collection = array();
	$user_collection = array();
	$content_collection = array();
	$address_collection = array();

	// Get the group IDs
	foreach($group_list as $group_id) {
		if($group_id != 0){
				// Count connected email addresses
		$email_query = db_select('pressrelease_email_connect', 'g')
			->condition ('group_id', $group_id)
			->fields ('g', array (
				'email_address_id',
				))
			->execute();

		// Handle the email group
		if(!isset($email_group)) {
			$email_group = $email_query->fetchAllKeyed(0,0);
				} else {
			$email_group_add = $email_query->fetchAllKeyed(0,0);
				foreach($email_group_add as $key => $value) {
					if(!in_array($value, $email_group)) {
						$email_group[$key] = $value;
					}
				}
			}

			// Count connected users
			$user_query = db_select('pressrelease_user_connect', 'g')
				->condition ('group_id', $group_id)
				->fields ('g', array (
					'user_id',
					))
				->execute();

				if(!isset($user_group)) {
					$user_group = $user_query->fetchAllKeyed(0,0);
					} else {
					$user_group_add = $user_query->fetchAllKeyed(0,0);
						foreach($user_group_add as $key => $value) {
							//drupal_set_message('<pre>value: '.print_r($value, TRUE).'</pre>');
							if(!in_array($value, $user_group)) {
								$user_group[$key] = $value;
							}
						}
					}
				// We now have our collections so prepare to send them.
				if(empty($email_group) && empty($user_group)){
					drupal_set_message(t('There are no email addresses in the groups you selected!'));
				} else {
					// Collect email addresses
					$collection = array();
					foreach($email_group as $email_address_id) {
						// Compile email addresses
						$query = db_select('pressrelease_email', 'e')
							->condition ('email_address_id', $email_address_id)
							->fields ('e', array (
								'email_address',
								))
							->execute();
							//$results = $query->fetchCol();
							$email = $query->fetchField();
							if(pressrelease_validate_email($email)){
								$email_collection[$email_address_id] = $email;
							} else {
								$invalid[] = $email;
							}
						}
					// Collect user email addresses
					foreach($user_group as $uid) {
						$user_data = user_load($uid);
						if($user_data){
							$email = $user_data->mail;
							if(pressrelease_validate_email($email)){
								$user_collection[$uid] = $email;
							} else {
								$invalid[] = $email;
							}
						}
					}
				}
			$content_collection = array();
			$content_count = '0';
			}
		}

	// Check the user-submitted email addresses
	$address_collection = array();
	foreach(explode(',', $addresses) as $email) {
		$email = trim($email);
		if(pressrelease_validate_email($email)){
			$address_collection[] = $email;
		} else {
			$invalid[] = $email;
		}
	}

	if(!empty($email_collection) || !empty($user_collection) || !empty($content_collection) || !empty($address_collection)){
		// Build the main email collection, filter results, eliminate duplicates
		$collection = array_values(array_filter(array_unique(array_merge($email_collection, $user_collection, $content_collection, $address_collection))));
		$invalid_count = count($invalid);
			if(!$invalid['0']){
				$invalid_count = 0;
			}
		$collection_count = count($collection);
		/*
		drupal_set_message('<pre>email_collection: '.print_r($email_collection, TRUE).'</pre><hr />');
		drupal_set_message('<pre>user_collection: '.print_r($user_collection, TRUE).'</pre><hr />');
		drupal_set_message('<pre>address_collection: '.print_r($address_collection, TRUE).'</pre><hr />');
		
		drupal_set_message('<pre>invalid addresses: '.print_r($invalid, TRUE).'</pre>');
		drupal_set_message('<pre>invalid_count: '.print_r($invalid_count, TRUE).'</pre><hr />');
		
		drupal_set_message('<pre>collection: '.print_r($collection, TRUE).'</pre>');
		drupal_set_message('<pre>collection_count: '.print_r($collection_count, TRUE).'</pre>');
		*/
		}
	return $collection;
}
