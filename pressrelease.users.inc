<?php

/**
 * Form for the Press Release User creation.
 */
function pressrelease_users_create_form($form, $form_state) {
	// Get the group_id from the URL
	$group_id = arg(6);

	if(!isset($form_state['group_id'])) {
		$form_state['group_id'] = $group_id;
		} else {
		//$form_state['group_id'] = $form_state['group_id'];
		$group_id = $form_state['group_id'];
		}

	// Add hidden form values for update
	$form['group_id'] = array(
		'#type' => 'hidden',
		'#value' => $group_id,
	);

	$form['#tree'] = TRUE;

  $form['users'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create users for this email group.'),
    '#collapsible' => TRUE,
		'#collapsed' => FALSE,
  );

	// Help for people who are lost
	//if(is_numeric($group_id) || $form_state['rebuild'] == TRUE) {
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for group_id
		$form['users']['group_id'] = array(
			'#type' => 'hidden',
			'#value' => $result['group_id'],
			);
	} else {
		//drupal_goto('admin/config/content/pressrelease/groups');
	}

	// Buid the form
	$form['users']['intro'] = array(
		'#type' => 'item',
		'#markup' => t('Users will be added to the <strong>@group</strong> group.', array('@group' => $result['title'])),
		);

	$form['users']['add_user'] = array(
		'#type' => 'textfield',
		'#title' => t('Create users'),
		//'#autocomplete_path' => 'pressrelease/user/autocomplete',
	);


///////////////////////////////////////////////////////////////////////

	// Check to see if access to roles is enabled.
	if(pressrelease_roles_access_check() == 1){
		$form['site_users_fieldset'] = array(
			'#type' => 'fieldset',
			'#title' => t('Add users from the site to email group.'),
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#prefix' => '<div id="site-users-fieldset-wrapper">',
			'#suffix' => '</div>',
		);



	// Setup the AJAX interaction
  if (empty($form_state['site_users_number'])) {
    $form_state['site_users_number'] = 1;
  }
  for ($i = 0; $i < $form_state['site_users_number']; $i++) {
		/*
    $form['site_users_fieldset']['name'][$i] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
    );
		*/
		$form['site_users_fieldset']['user_autocomplete'][$i] = array(
			'#type' => 'textfield',
			'#title' => t('Add site user'),
			'#autocomplete_path' => 'pressrelease/user/' . $group_id,
		);

  }
  $form['site_users_fieldset']['add_user'] = array(
    '#type' => 'submit',
    '#value' => t('Add one more'),
    '#submit' => array('pressrelease_add_user_of_group'),
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#ajax' => array(
      'callback' => 'pressrelease_user_to_group_callback',
      'wrapper' => 'site-users-fieldset-wrapper',
    ),
  );
  if ($form_state['site_users_number'] > 1) {
    $form['site_users_fieldset']['remove_user'] = array(
      '#type' => 'submit',
      '#value' => t('Remove one'),
      '#submit' => array('pressrelease_remove_user_of_group'),
      '#ajax' => array(
        'callback' => 'pressrelease_user_to_group_callback',
        'wrapper' => 'site-users-fieldset-wrapper',
      ),
    );
	}

	}

	// Check to see if the email module is present and that access to content is enabled.
	if(module_exists('email') && pressrelease_content_access_check() == 1) {
		$form['site_content_fieldset'] = array(
			'#type' => 'fieldset',
			'#title' => t('Add content with Email Fields to email group.'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);
		$form['site_content_fieldset']['site-content-autocomplete'] = array(
			'#type' => 'textfield',
			'#title' => t('Add content'),
			//'#autocomplete_path' => 'pressrelease/user/autocomplete/' . $allowed_roles_list . '/' . $disabled_roles_list,
			//'#autocomplete_path' => 'pressrelease/user/autocomplete/' . $group_id . '/',
			//'#autocomplete_path' => 'pressrelease/user/autocomplete/' . $group_id,
			'#autocomplete_path' => 'pressrelease/user/' . $group_id . '/' . $group_id . '/' . $group_id . '/' . $group_id . '/',
		);
	}

  $form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
		);

	$form['cancel'] = array(
		'#type' => 'button',
		'#value' => t('Cancel'),
		'#executes_submit_callback' => TRUE,
		'#submit' => array('pressrelease_form_cancel_groups'),
		);

	// Show the form header
  $form['intro'] = array(
		'#type' => 'item',
		//'#title' => t('List of Users Test.'),
		'#title' => pressrelease_autocomplete_user_test(),
	);
	return $form;
}

/**
 * Callback for both 'Site Users' ajax-enabled buttons.
 */
function pressrelease_user_to_group_callback($form, $form_state) {
  return $form['site_users_fieldset'];
}

/**
 * Submit handler for the "add-one-more" button.
 */
function pressrelease_add_user_of_group($form, &$form_state) {
  $form_state['site_users_number']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for the "remove one" button.
 */
function pressrelease_remove_user_of_group($form, &$form_state) {
  if ($form_state['site_users_number'] > 1) {
    $form_state['site_users_number']--;
  }
  $form_state['rebuild'] = TRUE;
}



/////////////////////////////////////////////////////////////////////



/**
 * Form for the Press Release Email Address creation.
 */
function pressrelease_users_email_add($form, $form_state) {
//drupal_set_message('email address add');

// Get the group_id from the URL
$group_id = arg(7);
if(!isset($form_state['group_id'])) {
	$form_state['group_id'] = $group_id;
	} else {
	//$form_state['group_id'] = $form_state['group_id'];
	$group_id = $form_state['group_id'];
	}

	// Add hidden form values for update
	$form['group_id'] = array(
		'#type' => 'hidden',
		'#value' => $group_id,
	);

	$form['#tree'] = TRUE;

	$form['site_users_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => t('Add users to this email group.'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
		'#prefix' => '<div id="site-users-fieldset-wrapper">',
		'#suffix' => '</div>',
	);

	// Help for people who are lost
	//if(is_numeric($group_id) || $form_state['rebuild'] == TRUE) {
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for group_id
		$form['users']['group_id'] = array(
			'#type' => 'hidden',
			'#value' => $result['group_id'],
			);
	} else {
		//drupal_goto('admin/config/content/pressrelease/groups');
	}

	// Buid the form
	$form['site_users_fieldset']['intro'] = array(
		'#type' => 'item',
		'#markup' => t('Users will be added to the !group_link group.', array('!group_link' => l($result['title'], 'admin/config/content/pressrelease/users/email/list/'. $group_id))),
		);

	// Setup the AJAX interaction
  if (empty($form_state['site_users_number'])) {
    $form_state['site_users_number'] = 1;
  }
  for ($i = 0; $i < $form_state['site_users_number']; $i++) {
		$form['site_users_fieldset']['email_autocomplete'][$i] = array(
			'#type' => 'textfield',
			'#title' => t('Email Address'),
			'#autocomplete_path' => 'pressrelease/user/email/autocomplete/' . $group_id,
			//'#required' => TRUE,
		);
  }

  $form['site_users_fieldset']['add_user'] = array(
    '#type' => 'submit',
    '#value' => t('Add one more'),
    '#submit' => array('pressrelease_add_user_of_group'),
    // See the examples in ajax_example.module for more details on the
    // properties of #ajax.
    '#ajax' => array(
      'callback' => 'pressrelease_user_to_group_callback',
      'wrapper' => 'site-users-fieldset-wrapper',
    ),
  );
  if ($form_state['site_users_number'] > 1) {
    $form['site_users_fieldset']['remove_user'] = array(
      '#type' => 'submit',
      '#value' => t('Remove one'),
      '#submit' => array('pressrelease_remove_user_of_group'),
      '#ajax' => array(
        'callback' => 'pressrelease_user_to_group_callback',
        'wrapper' => 'site-users-fieldset-wrapper',
      ),
    );
	}
/*
	// Groups section of this form
	$form['groups'] = array(
		'#type' => 'fieldset',
		'#title' => t('Available groups.'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
		'#prefix' => '<div id="site-groups-fieldset-wrapper">',
		'#suffix' => '</div>',
		);

	// Build checkbox options - show all groups
	// Get all of the items for the row
	$query = db_select('pressrelease_group', 'g')
		->fields ('g', array (
			'group_id',
			'title',
			));
	$results = $query->execute();

	// Build full list array
	$groups = array();
  foreach($results as $result) {
		$groups[$result->group_id] = $result->title;
  }
	// Get the default from the group_id
	$default = array($group_id);

	// Show the available groups and defaults
  $form['groups']['group_member']['#tree'] = TRUE;
  $form['groups']['group_member'] = array(
    '#type' => 'checkboxes',
    '#options' => $groups,
		'#default_value' => $default,
		'#description' => t('Users can be added to or removed from groups here.'),
	  );
*/
  $form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
		);

	$form['cancel'] = array(
		'#type' => 'button',
		'#value' => t('Cancel'),
		'#executes_submit_callback' => TRUE,
		'#submit' => array('pressrelease_form_cancel_groups'),
		);

return $form;
}

/**
 * Submit handler for the bundle configuration form.
 */
function pressrelease_users_email_add_submit($form, &$form_state, $group_id = '') {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');

//drupal_set_message('<pre>form_values:<br>'.print_r($form_values, TRUE).'</pre>');

	foreach($form_values['site_users_fieldset']['email_autocomplete'] as $email) {
		if($email){

		// Get the default values of the form
		$result = db_select('pressrelease_email', 'e')
			->fields('e')
			->condition('email_address', $email, '=')
			->execute()
			->fetchAssoc();

		// Verify that we're putting the correct info in the correct spots
		if(!$result['email_address_id']){
			$email_address_id = db_insert('pressrelease_email')
				->fields(array(
					'email_address' => $email,
					'created' => $timestamp,
					'modified' => $timestamp
				))
				->execute();
			} else {
				$email_address_id = $result['email_address_id'];
			}

			db_insert('pressrelease_email_connect')
				->fields(array(
					'group_id' => $form_values['group_id'],
					'email_address_id' => $email_address_id
				))
				->execute();
		}
	}
	drupal_set_message(t('Email addresses were added'));
	// Send the user back to the list
	$form_state['redirect'] = 'admin/config/content/pressrelease/users/email/list/' . $form_values['group_id'];
}


/**
 * Form for the Press Release Email Address list.
 */
function pressrelease_users_email_list_all($group_id = '0') {
	$return_link = l(t('Group List'), 'admin/config/content/pressrelease/groups');
	$html = '';
	$html .= t('Return to the !return_link.', array('!return_link' => $return_link));

	$header = array(
		array('data' => 'Email Addresses', 'field' => 'email_address', 'sort' => 'asc'),
		array('data' => 'Modified', 'field' => 'modified', 'sort' => 'asc'),
		array('data' => 'Edit'),
		array('data' => 'Remove from Group or Delete'),
		);

	// Get all of the items for the row
	$query = db_select('pressrelease_email', 'e')
		->extend('PagerDefault')
		->limit(10)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('e', array (
			'email_address_id',
			'email_address',
			'created',
			'modified',
			));
	$results = $query->execute();

	// Create the table rows
  $rows = array();
  foreach($results as $result) {
		// now build the rows for the table
    $rows[] = array(
        l($result->email_address, 'admin/config/content/pressrelease/users/email/edit/'. $group_id .'/'. $result->email_address_id),
        format_date($result->modified),
        l('Edit', 'admin/config/content/pressrelease/users/email/edit/'. $group_id .'/'. $result->email_address_id),
        l('Remove/Delete', 'admin/config/content/pressrelease/users/email/delete/'. $group_id .'/'. $result->email_address_id),
     		);
  	}
		// Build the table theme
		$html .= theme('table',
			array(
			'description' => array('some stuff'),
				'header' => $header,
				'rows' => $rows,
				'sticky' => TRUE,
				'empty' => t('No addresses found for this group.'),
			)
		);
	// Add Pager
		$html .= theme('pager',
			array(
				'tags' => array()
			)
		);
	return($html);
}




/**
 * Form for the Press Release Email Address list.
 */
function pressrelease_users_email_list($group_id = '') {

	// Help for people who are lost
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
	} else {
		drupal_goto('admin/config/content/pressrelease/groups');
	}

	//$return_link = l($result['title'], 'admin/config/content/pressrelease/groups/edit/'. $group_id);
	$return_link = l($result['title'], 'admin/config/content/pressrelease/groups');
	$html = '';
	$html .= t('Email addresses in the !return_link group:', array('!return_link' => $return_link));

	$header = array(
		array('data' => 'Email Addresses', 'field' => 'email_address', 'sort' => 'asc'),
		array('data' => 'Modified', 'field' => 'modified', 'sort' => 'asc'),
		array('data' => 'Edit'),
		//array('data' => 'Remove From Group'),
		array('data' => 'Remove from Group or Delete'),
		);

/*
	// Get all of the items for the row
	$connect_query = db_select('pressrelease_email_connect', 'c')
		->condition('group_id', $group_id)
		->fields ('c', array (
			'email_address_id',
			))
		->execute();
	$connect_results = $connect_query->fetchCol();

  foreach($connect_results as $connect_result) {
		$connect_result_array[] = $connect_result;
	}
*/

	// Get all of the items for the row
	$query = db_select('pressrelease_email', 'e')
		->extend('PagerDefault')
		->limit(10)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('e', array (
			'email_address_id',
			'email_address',
			'created',
			'modified',
			));
		$subquery = db_select('pressrelease_email_connect', 'c')
			->fields ('c', array ('email_address_id',))
			->condition('group_id', $group_id);
	$query->condition('email_address_id', $subquery, 'IN');
	$results = $query->execute();

	// Create the table rows
  $rows = array();
  foreach($results as $result) {
		// now build the rows for the table
    $rows[] = array(
				//$result->group_id,
        l($result->email_address, 'admin/config/content/pressrelease/users/email/edit/'. $group_id .'/'. $result->email_address_id),
        format_date($result->modified),
        l('Edit', 'admin/config/content/pressrelease/users/email/edit/'. $group_id .'/'. $result->email_address_id),
        //l('Remove', 'admin/config/content/pressrelease/users/email/remove/'. $group_id .'/'. $result->email_address_id),
        l('Remove/Delete', 'admin/config/content/pressrelease/users/email/delete/'. $group_id .'/'. $result->email_address_id),
     		);
  	}
		// Build the table theme
		$html .= theme('table',
			array(
			'description' => array('some stuff'),
				'header' => $header,
				'rows' => $rows,
				'sticky' => TRUE,
				'empty' => t('No addresses found for this group.'),
			)
		);
	// Add Pager
		$html .= theme('pager',
			array(
				'tags' => array()
			)
		);
	return($html);
}

/**
 * Form for the Press Release Email Address edit.
 */
function pressrelease_users_email_edit($form, $form_state, $group_id = '', $email_address_id = '') {
	// Get the group_id from the URL
	//$email_address_id = arg(8);
	if(!isset($form_state['email_address_id'])) {
		$form_state['email_address_id'] = $email_address_id;
		} else {
		$email_address_id = $form_state['email_address_id'];
		}

	// Add hidden form values for update
	$form['email_address_id'] = array(
		'#type' => 'hidden',
		'#value' => $email_address_id,
		);

	if($group_id){
	// Add hidden form values for update
	$form['group_id'] = array(
		'#type' => 'hidden',
		'#value' => $group_id,
		);
	}

	$form['#tree'] = TRUE;

	$form['site_users_fieldset'] = array(
		'#type' => 'fieldset',
		'#title' => t('Edit this email address.'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
		);

	// Help for people who are lost
	if(is_numeric($email_address_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_email', 'e')
			->fields('e')
			->condition('email_address_id', $email_address_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for group_id
		$form['users']['email_address_id'] = array(
			'#type' => 'hidden',
			'#value' => $result['email_address_id'],
			);
	}
	// Buid the form
	$form['site_users_fieldset']['intro'] = array(
		'#type' => 'item',
		'#markup' => t('Please enter a valid email address.'),
		);
	$form['site_users_fieldset']['email_address'] = array(
		'#type' => 'textfield',
		'#title' => t('Email Address'),
		'#default_value' => $result['email_address'],
		'#required' => TRUE,
		);

	// Groups section of this form
	$form['groups'] = array(
		'#type' => 'fieldset',
		'#title' => t('Available groups.'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
		'#prefix' => '<div id="site-groups-fieldset-wrapper">',
		'#suffix' => '</div>',
		);

	// Build checkbox options - show all groups
	// Get all of the items for the row
	$query = db_select('pressrelease_group', 'g')
		->fields ('g', array (
			'group_id',
			'title',
			));
	$group_items = $query->execute();

	// Build full list array
	$groups = array();
  foreach($group_items as $group_item) {
		$groups[$group_item->group_id] = $group_item->title;
  	}

	// Get the default values
	$query = db_select('pressrelease_group', 'g')
		->fields ('g', array (
			'group_id',
			'title',
			));
		$subquery = db_select('pressrelease_email_connect', 'c')
			->fields ('c', array ('group_id',))
			->condition('email_address_id', $email_address_id);
	$query->condition('group_id', $subquery, 'IN');
	$group_defaults = $query->execute();

	// Build defaults array
	$defaults = array();
  foreach($group_defaults as $group_default) {
		$defaults[$group_default->group_id] = $group_default->title;
  	}
	// Show the available groups and defaults
  $form['groups']['group_member']['#tree'] = TRUE;
  $form['groups']['group_member'] = array(
    '#type' => 'checkboxes',
    '#options' => $groups,
		'#default_value' => array_keys($defaults),
		'#description' => t('Users can be added to or removed from groups here.'),
  );

	// submit the form
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
		);
	$form['cancel'] = array(
		'#type' => 'button',
		'#value' => t('Cancel'),
		'#executes_submit_callback' => TRUE,
		'#submit' => array('pressrelease_form_cancel_groups'),
		);
return $form;
}
/**
 * Submit handler for the user edit.
 */
function pressrelease_users_email_edit_submit($form, &$form_state) {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');
	$groups = $form_values['groups']['group_member'];

	foreach($groups as $group_id => $group) {
		switch ($group) {
			case 0:
				//drupal_set_message('<pre>not assigned: '.print_r($group_id, TRUE).'</pre>');
				// Check to see if the group was removed
				$query = db_select('pressrelease_email_connect', 'c')
					->fields ('c', array (
						'email_connect_id',
						))
					->condition('email_address_id', $form_values['email_address_id'])
					->condition('group_id', $group_id)
					->execute();
				$check = $query->fetchField();
				//drupal_set_message('<pre>check: '.print_r($check, TRUE).'</pre>');
					if($check) {
						//drupal_set_message('<pre>DELETE! '.print_r($check, TRUE).'</pre>');
						// Delete the email address connections
						$num_connect_deleted = db_delete('pressrelease_email_connect')
							->condition('email_address_id', $form_values['email_address_id'])
							->condition('group_id', $group_id)
							->execute();
					}
				break;
			default:
				//drupal_set_message('<pre>assigned: '.print_r($group_id, TRUE).'</pre>');
				// Check to see if the group is already added
				$query = db_select('pressrelease_email_connect', 'c')
					->fields ('c', array (
						'email_connect_id',
						))
					->condition('email_address_id', $form_values['email_address_id'])
					->condition('group_id', $group_id)
					->execute();
				$check = $query->fetchField();

				//drupal_set_message('<pre>check: '.print_r($check, TRUE).'</pre>');
					if(!$check) {
						//drupal_set_message('<pre>INSERT! '.print_r($check, TRUE).'</pre>');
						$email_address_id = db_insert('pressrelease_email_connect')
							->fields(array(
								'group_id' => $group_id,
								'email_address_id' => $form_values['email_address_id']
							))
							->execute();
					}
				break;
		}
		//drupal_set_message('<hr/>');
	}

	// Update email address
	$update = db_update('pressrelease_email')
	->fields(array(
		'email_address' => $form_values['site_users_fieldset']['email_address'],
		'modified' => $timestamp,
		))
	->condition('email_address_id', $form_values['email_address_id'], '=')
	->execute();
	drupal_set_message(t('Email Address was updated.'));

	// Send the user back to the list
	if($form_values['group_id'] == 0) {
		$form_state['redirect'] = 'admin/config/content/pressrelease/users/email/list';
	} else {
		$form_state['redirect'] = 'admin/config/content/pressrelease/users/email/list/' . $form_values['group_id'];
	}
}




/**
 * Form for the Press Release Email Address delete.
 */
function pressrelease_users_email_delete($form, $form_state, $group_id = '', $email_address_id = '') {
	// Help for people who are lost
	if(is_numeric($email_address_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_email', 'e')
			->fields('e')
			->condition('email_address_id', $email_address_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for email_address_id
		$form['values']['email_address_id'] = array(
			'#type' => 'hidden',
			'#value' => $result['email_address_id'],
			);
	} else {
		drupal_goto('admin/config/content/pressrelease/users/email/list/' . $form_values['group_id']);
	}

	// Another default value - group_id
	$form['values']['group_id'] = array(
		'#type' => 'hidden',
		'#value' => $group_id,
	);
	// Build the form
  $form['remove'] = array(
		'#type' => 'fieldset',
		'#title' => t('Address: @email_address', array('@email_address' => $result['email_address'])),
	);
  $form['remove']['intro'] = array(
		'#type' => 'item',
		'#title' => t('Either remove the address from the group(s) it is connected to or delete it entirely.'),
		'#markup' => t('This user is connected to the following groups:'),
	);

	// Build the table of connected groups
  $header = array(
    'title' => t('Group Name'),
    'description' => t('Description'),
  );

	// Get all of the items for the row
	$query = db_select('pressrelease_group', 'g')
		->extend('PagerDefault')
		->limit(5)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('g', array (
			'group_id',
			'title',
			'description',
			'modified',
			));
		$subquery = db_select('pressrelease_email_connect', 'c')
			->fields ('c', array ('group_id',))
			->condition('email_address_id', $email_address_id);
	$query->condition('group_id', $subquery, 'IN');
	$results = $query->execute();

	// Create the table rows
  $rows = array();
  foreach($results as $result) {
		// now build the rows for the table
    $rows[$result->group_id] = array(
				'group_id' => t($result->group_id),
        'title' => t($result->title),
        'description' => t($result->description),
			);
  	}

	$add_link = l('Add this user to a group.', 'admin/config/content/pressrelease/users/email/edit/0/'. $email_address_id);
  $form['remove']['groups'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No groups found. !add_link', array('!add_link' => $add_link)),
  );
	// Add Pager
	$form['remove']['pager'] = theme('pager',
		array(
			'tags' => array()
		)
	);
  $form['remove']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Remove From Groups'),
    '#submit' => array('pressrelease_remove_email_from_group'),
	);

	// Second half of the form
  $form['delete'] = array(
		'#type' => 'fieldset',
		'#title' => t('Delete this email address and remove them from all groups.'),
	);
  $form['delete']['intro'] = array(
		'#type' => 'item',
		'#markup' => t('Warning: There is no undo for this action!'),
	);
  $form['delete']['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Delete Address and Remove from Groups')
	);

	return $form;
}
/**
 * Submit handler for the email address confirm delete.
 */
function pressrelease_users_email_delete_submit($form, &$form_state) {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');

	//drupal_set_message('<pre>form_values:<br>'.print_r($form_values, TRUE).'</pre>');

	// Delete the email address connections
	$num_connect_deleted = db_delete('pressrelease_email_connect')
		->condition('email_address_id', $form_values['email_address_id'])
		->execute();
	// Delete the email address
	$num_address_deleted = db_delete('pressrelease_email')
		->condition('email_address_id', $form_values['email_address_id'])
		->execute();

	// Let the user know it's gone
	drupal_set_message('Email Address was deleted and removed from groups');
	// Send them to the list page
	$form_state['redirect'] = 'admin/config/content/pressrelease/users/email/list/' . $form_values['group_id'];
}
/**
 * Submit handler for the email address remove from list.
 */
function pressrelease_remove_email_from_group($form, &$form_state) {
	$form_values = $form_state['values'];
	$timestamp = strtotime('now');

	foreach($form_values['groups'] as $group_id) {
		if($group_id != 0) {
			// Delete the email address connections
			$num_connect_deleted = db_delete('pressrelease_email_connect')
				->condition('email_address_id', $form_values['email_address_id'])
				->condition('group_id', $group_id)
				->execute();
		}
	}
	drupal_set_message(t('Email Addresses were removed from groups.'));
	$form_state['redirect'] = 'admin/config/content/pressrelease/users/email/list/' . $form_values['group_id'];
}
















/**
 * Form for the Press Release Site User add.
 */
function pressrelease_users_site_add($form, $form_state, $group_id = '') {
// Get the group_id from the URL
//$group_id = arg(7);
if(!isset($form_state['group_id'])) {
	$form_state['group_id'] = $group_id;
	} else {
	//$form_state['group_id'] = $form_state['group_id'];
	$group_id = $form_state['group_id'];
	}

	// Help for people who are lost
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for group_id
		$form['group_id'] = array(
			'#type' => 'hidden',
			'#value' => $result['group_id'],
			);
	} else {
		//drupal_goto('admin/config/content/pressrelease/groups');
	}

	$form['#tree'] = TRUE;

	// Check to see if access to roles is enabled.
	if(pressrelease_roles_access_check() == 1){
		$form['site_users_fieldset'] = array(
			'#type' => 'fieldset',
			'#title' => t('Add users from the site to email group.'),
			'#collapsible' => TRUE,
			'#collapsed' => FALSE,
			'#prefix' => '<div id="site-users-fieldset-wrapper">',
			'#suffix' => '</div>',
			);

		// Buid the form
		$form['site_users_fieldset']['intro'] = array(
			'#type' => 'item',
			'#markup' => t('Users will be added to the <strong>@group</strong> group.', array('@group' => $result['title'])),
		);
	
			// Setup the AJAX interaction
			if (empty($form_state['site_users_number'])) {
				$form_state['site_users_number'] = 1;
			}
			for ($i = 0; $i < $form_state['site_users_number']; $i++) {
				$form['site_users_fieldset']['email_autocomplete'][$i] = array(		
					'#type' => 'textfield',
					'#title' => t('Add site user'),
					'#autocomplete_path' => 'pressrelease/user/site/' . $group_id,
				);
			}
		$form['site_users_fieldset']['add_user'] = array(
			'#type' => 'submit',
			'#value' => t('Add one more'),
			'#submit' => array('pressrelease_add_user_of_group'),
			'#ajax' => array(
				'callback' => 'pressrelease_user_to_group_callback',
				'wrapper' => 'site-users-fieldset-wrapper',
			),
		);
		if ($form_state['site_users_number'] > 1) {
			$form['site_users_fieldset']['remove_user'] = array(
				'#type' => 'submit',
				'#value' => t('Remove one'),
				'#submit' => array('pressrelease_remove_user_of_group'),
				'#ajax' => array(
					'callback' => 'pressrelease_user_to_group_callback',
					'wrapper' => 'site-users-fieldset-wrapper',
				),
			);
		}
	}

	// submit the form
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
		);
	$form['cancel'] = array(
		'#type' => 'button',
		'#value' => t('Cancel'),
		'#executes_submit_callback' => TRUE,
		'#submit' => array('pressrelease_form_cancel_groups'),
		);
		
return $form;
}

/**
 * Submit handler for the bundle configuration form.
 */
function pressrelease_users_site_add_submit($form, &$form_state, $group_id = '') {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');
	// Enter each user
	foreach($form_values['site_users_fieldset']['email_autocomplete'] as $name) {
		if($name){
			// We need the uid
			$query = db_select('users', 'u')
				->fields('u', array(
					'uid',
					))
				->condition('name', $name)
				->execute();
			$uid = $query->fetchField();

			// Do a quick check to see if the user is already connected
			$query = db_select('pressrelease_user_connect', 'c')
				->fields('c', array(
					'user_connect_id',
					))
				->condition(db_and()->condition('user_id', $uid, '=')
				->condition('group_id', $form_values['group_id']), '=')
				->execute();
			$check = $query->fetchField();

			// if the user isn't connected do it now
			if(!$check){
				$insert = db_insert('pressrelease_user_connect')
				->fields(array(
					'group_id' => $form_values['group_id'],
					'user_id' => $uid,
				))
				->execute();
			}
		}
	}
	drupal_set_message(t('Email addresses were connected'));
	// Send the user back to the list
	$form_state['redirect'] = 'admin/config/content/pressrelease/users/site/list/' . $form_values['group_id'];
}


/**
 * Form for the Press Release User list.
 */
function pressrelease_users_site_list($group_id = '') {
//drupal_set_message('site list');

	// Help for people who are lost
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
	} else {
		drupal_goto('admin/config/content/pressrelease/groups');
	}

	//$return_link = l($result['title'], 'admin/config/content/pressrelease/groups/edit/'. $group_id);
	$return_link = l($result['title'], 'admin/config/content/pressrelease/groups');
	$html = '';
	$html .= t('Site users connected to the !return_link group:', array('!return_link' => $return_link));

	$header = array(
		array('data' => 'Name', 'field' => 'name', 'sort' => 'asc'),
		array('data' => 'Email Addresses', 'field' => 'email_address', 'sort' => 'asc'),
		array('data' => 'Edit/Remove'),
		//array('data' => 'Remove From Group'),
		//array('data' => 'Remove from Group or Delete'),
		);

	// Get all of the items for the row
	$query = db_select('users', 'u')
		->extend('PagerDefault')
		->limit(10)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('u', array (
			'uid',
			'name',
			'mail',
			'created',
			));

		$subquery = db_select('pressrelease_user_connect', 'c')
			->fields ('c', array ('user_id',))
			->condition('group_id', $group_id);
	$query->condition('uid', $subquery, 'IN');

	$results = $query->execute();

	// Create the table rows
  $rows = array();
  foreach($results as $result) {
		// now build the rows for the table
    $rows[] = array(
				//$result->group_id,
        //l($result->name, 'admin/config/content/pressrelease/users/site/edit/'. $group_id .'/'. $result->uid),
        l($result->name, 'user/' . $result->uid . '/edit', array('query'=> array('destination' => 'admin/config/content/pressrelease/users/site/list/'. $group_id))),
        l($result->mail, 'user/' . $result->uid . '/edit', array('query'=> array('destination' => 'admin/config/content/pressrelease/users/site/list/'. $group_id))),
        l('Edit/Remove', 'user/' . $result->uid . '/edit', array('query'=> array('destination' => 'admin/config/content/pressrelease/users/site/list/'. $group_id))),
        //l('Remove', 'admin/config/content/pressrelease/users/site/remove/'. $group_id .'/'. $result->uid),
     		);
  	}
		// Build the table theme
		$html .= theme('table',
			array(
			'description' => array('some stuff'),
				'header' => $header,
				'rows' => $rows,
				'sticky' => TRUE,
				'empty' => t('No addresses found for this group.'),
			)
		);
	// Add Pager
		$html .= theme('pager',
			array(
				'tags' => array()
			)
		);
	return($html);
}



/**
 * Form for the Press Release User remove.
 */
function pressrelease_users_site_remove($form, $form_state) {
//drupal_set_message('site remove');
}





/**
 * Form for the Press Release Content User add.
 */
function pressrelease_users_content_add($form, $form_state) {
//drupal_set_message('content add');
}
/**
 * Form for the Press Release Content User list.
 */
function pressrelease_users_content_list($form, $form_state) {
//drupal_set_message('content list');
}
/**
 * Form for the Press Release Content User remove.
 */
function pressrelease_users_content_remove($form, $form_state) {
//drupal_set_message('content remove');
}





/////////////////////////////////////////////////////////////////////////////



function pressrelease_user_autocomplete($group_id, $string) {
	$matches = array();
	// Get the allowed role list for this group
	$roles_access = pressrelease_get_role_acccess($group_id);
	$allowed_roles_list = $roles_access['allowed'];
	$disabled_roles_list = $roles_access['disabled'];

	// Create autocomplete query
	if(!empty($allowed_roles_list)) {
		$sub_query_1 = '';
		$sub_query_2 = '';
	
		if(!empty($allowed_roles_list)) {
			$sub_query_1 = ' AND uid IN (SELECT DISTINCT uid FROM users_roles';
			$i=1;
			foreach($allowed_roles_list as $value){
				if($i==1) {
					$sub_query_1 .= ' WHERE rid = ' . $value;
					$i++;
				} else {
					$sub_query_1 .= ' OR rid = ' . $value;
				}
			}
			$sub_query_1 .= ')';
		} else {
			$sub_query_1 .= '';
		}
	
		if(!empty($disabled_roles_list)) {
			$sub_query_2 = ' AND uid NOT IN (SELECT DISTINCT uid FROM users_roles';
			$i=1;
			foreach($disabled_roles_list as $value){
				if($i==1) {
					$sub_query_2 .= ' WHERE rid = ' . $value;
					$i++;
				} else {
					$sub_query_2 .= ' OR rid = ' . $value;
				}
			}
			$sub_query_2 .= ')';
		} else {
			$sub_query_2 .= '';
		}

		$start_query = "SELECT * FROM users WHERE name LIKE '%" . $string . "%'";
		$query = $start_query . $sub_query_1 . $sub_query_2;
		$result = db_query($query);
	}

/*
	//$result = db_query("SELECT name FROM users WHERE lower(name) LIKE '%".$string."%' LIMIT 10");
	$result = db_select('users', 'u')
		->fields('u', array('uid', 'name'))
		->condition('u.name', '%'.$string.'%', 'LIKE')
		->range(0, 10)
		->execute();
*/
		// add matches to $matches 
		foreach ($result as $row) {
			//$matches[$row->name . ' [id:' . $row->uid . ']'] = check_plain($row->name) . " [id:" . $row->uid . "]";
			$matches[$row->name] = check_plain($row->name);
		}
  // return for JS
  drupal_json_output($matches);
	//}
}



function pressrelease_site_autocomplete($group_id, $string) {
	$matches = array();
	// Get the allowed role list for this group
	$roles_access = pressrelease_get_role_acccess($group_id);
	$allowed_roles_list = $roles_access['allowed'];
	$disabled_roles_list = $roles_access['disabled'];

	// Create autocomplete query
	if(!empty($allowed_roles_list)) {
		$sub_query_1 = '';
		$sub_query_2 = '';
	
		if(!empty($allowed_roles_list)) {
			$sub_query_1 = ' AND uid IN (SELECT DISTINCT uid FROM users_roles';
			$i=1;
			foreach($allowed_roles_list as $value){
				if($i==1) {
					$sub_query_1 .= ' WHERE rid = ' . $value;
					$i++;
				} else {
					$sub_query_1 .= ' OR rid = ' . $value;
				}
			}
			$sub_query_1 .= ')';
		} else {
			$sub_query_1 .= '';
		}

		$start_query = "SELECT * FROM users WHERE name LIKE '%" . $string . "%'";
		$query = $start_query . $sub_query_1;
		$result = db_query($query);
	}

/*
	//$result = db_query("SELECT name FROM users WHERE lower(name) LIKE '%".$string."%' LIMIT 10");
	$result = db_select('users', 'u')
		->fields('u', array('uid', 'name'))
		->condition('u.name', '%'.$string.'%', 'LIKE')
		->range(0, 10)
		->execute();
*/
		// add matches to $matches 
		foreach ($result as $row) {
			//$matches[$row->name . ' [id:' . $row->uid . ']'] = check_plain($row->name) . " [id:" . $row->uid . "]";
			$matches[$row->name] = check_plain($row->name);
		}
  // return for JS
  drupal_json_output($matches);
	//}
}



// Private function to autocomplete email searches
function pressrelease_user_email_autocomplete($group_id, $string) {
	$matches = array();
	// Get all of the items for the row
	$query = db_select('pressrelease_email', 'e')
		->fields ('e', array (
			'email_address_id',
			'email_address',
			))
			->condition('e.email_address', '%'.$string.'%', 'LIKE')
			->range(0, 10);
		$subquery = db_select('pressrelease_email_connect', 'c')
			->fields ('c', array (
				'email_address_id',
				))
			->condition('group_id', $group_id);
	$query->condition('email_address_id', $subquery, 'NOT IN');
	$result = $query->execute();

	// add matches to $matches 
	foreach ($result as $row) {
		//$matches[$row->name . ' [id:' . $row->uid . ']'] = check_plain($row->name) . " [id:" . $row->uid . "]";
		$matches[$row->email_address] = check_plain($row->email_address);
	}
  // return for JS
  drupal_json_output($matches);
}




function pressrelease_autocomplete_user_test($string= '') {
$group_id = arg(6);

$roles_access = pressrelease_get_role_acccess($group_id);
//drupal_set_message('<pre>roles_access:<br>'.print_r($roles_access, TRUE).'</pre>');

$allowed_roles_list = $roles_access['allowed'];
$disabled_roles_list = $roles_access['disabled'];

	$matches = array();
	//drupal_set_message('<pre>allowed_roles_list:<br>'.print_r($allowed_roles_list, TRUE).'</pre>');
	//drupal_set_message('<pre>disabled_roles_list:<br>'.print_r($disabled_roles_list, TRUE).'</pre>');
	//$enabled_roles = variable_get('pressrelease_roles_enable', array());
	//$available_roles = group_get_role_types(variable_get('pressrelease_roles', array()));
	//$result = db_query("SELECT name FROM users WHERE lower(name) LIKE '%".$string."%' LIMIT 10");

	if(!empty($allowed_roles_list)) {
		$sub_query_1 = '';
		$sub_query_2 = '';
	
		if(!empty($allowed_roles_list)) {
			$sub_query_1 = ' AND uid IN (SELECT DISTINCT uid FROM users_roles';
			$i=1;
			foreach($allowed_roles_list as $value){
				if($i==1) {
					$sub_query_1 .= ' WHERE rid = ' . $value;
					$i++;
				} else {
					$sub_query_1 .= ' OR rid = ' . $value;
				}
			}
			$sub_query_1 .= ')';
		} else {
			$sub_query_1 .= '';
		}
	
		if(!empty($disabled_roles_list)) {
			$sub_query_2 = ' AND uid NOT IN (SELECT DISTINCT uid FROM users_roles';
			$i=1;
			foreach($disabled_roles_list as $value){
				if($i==1) {
					$sub_query_2 .= ' WHERE rid = ' . $value;
					$i++;
				} else {
					$sub_query_2 .= ' OR rid = ' . $value;
				}
			}
			$sub_query_2 .= ')';
		} else {
			$sub_query_2 .= '';
		}
		//drupal_set_message('<pre>sub_query_2:<br>'.print_r($sub_query_2, TRUE).'</pre>');	
	
		$start_query = "SELECT * FROM users WHERE name LIKE '%" . $string . "%'";
		$query = $start_query . $sub_query_1 . $sub_query_2;
		$result = db_query($query);
	
	
		//drupal_set_message('<pre>query:<br>'.print_r($query, TRUE).'</pre>');
	
		// add matches to $matches 
		foreach ($result as $row) {
			$matches[$row->name] = check_plain($row->name);
		}
	
		//drupal_set_message('<pre>search result:<br>'.print_r(user_load($row->uid), TRUE).'</pre>');	
		//drupal_set_message('<pre>matches:<br>'.print_r($matches, TRUE).'</pre>');
		//drupal_set_message('<pre>enabled roles:<br>'.print_r(user_roles(), TRUE).'</pre>');
	
		// return for JS
		//drupal_json_output($matches);
	}
}







// Private Function to get enabled and disabled role lists for a specific group
function pressrelease_get_role_acccess($group_id) {

	$allowed_roles_list = '';
	$disabled_roles_list = '';

	// Help for people who are lost
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();

		$disabled_roles = unserialize($result['disable_roles']);
		$available_roles = group_get_role_types(variable_get('pressrelease_roles', array()));
		$available_roles_flip = array();
		if(!empty($available_roles)) {
			$available_roles_flip = array_flip($available_roles);
		}
		$disabled_roles_list = array();
		$allowed_roles_list = array();

		// Filter disabled roles list
		foreach($disabled_roles as $value) {
			if($value != 0) {
				$disabled_roles_list[] = $value;
			}
		}
		// Create allowed roles list
		foreach($available_roles_flip as $value) {
			if(!in_array($value, $disabled_roles_list)) {
				$allowed_roles_list[] = $value;
			}
		}
	}
	$results = array('allowed' => $allowed_roles_list, 'disabled' => $disabled_roles_list);
return $results;
}
