<?php

// @todo: Add users, connect users, connect content



/**
 * Form for the Newsletter Groups configuration.
 */
function pressrelease_groups_list() {
	$header = array(
		//array('data' => 'ID'),
		array('data' => 'Title', 'field' => 'title', 'sort' => 'asc'),
		array('data' => 'Email Addresses'),
		array('data' => 'Site Users'),
		array('data' => 'Content'),
		array('data' => 'Description'),
		array('data' => 'Created', 'field' => 'created'),
		array('data' => 'Edit'),
		array('data' => 'Delete'),
		);
	// Get all of the items for the row
	$query = db_select('pressrelease_group', 'g')
		->condition('deleted', 0)
		->extend('PagerDefault')
		->limit(100)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('g', array (
			'group_id',
			'title',
			'description',
			'created',
			'modified',
			'deleted',
			//'thumbnail_fid',
			//'html',
			));
	$results = $query->execute();
	// Create the table rows
  $rows = array();
  foreach($results as $result) {
		// display the image thumbnail
/*		$f = file_load($template->thumbnail_fid);
		$url = theme("image_formatter", array("image_style" => "thumbnail", "item" => array("uri" => $f->uri, "title" => $f->filename, "alt" => $template->title)));
*/
		// now build the rows for the table
    $rows[] = array(
				//$result->group_id,
        l($result->title, 'admin/config/content/pressrelease/groups/edit/'. $result->group_id),
        l('Add', 'admin/config/content/pressrelease/users/email/list/' . $result->group_id . '/add/') . '/' . l('List', 'admin/config/content/pressrelease/users/email/list/'. $result->group_id),
        l('Add', 'admin/config/content/pressrelease/users/site/add/'. $result->group_id) . '/' . l('List', 'admin/config/content/pressrelease/users/site/list/'. $result->group_id),
        l('Add', 'admin/config/content/pressrelease/users/content/add/'. $result->group_id) . '/' . l('List', 'admin/config/content/pressrelease/users/content/list/'. $result->group_id),
        //truncate_utf8($result->description, 200, TRUE, TRUE),
				text_summary($result->description, 100),
        format_date($result->modified),
        l('Edit', 'admin/config/content/pressrelease/groups/edit/'. $result->group_id),
        l('Delete', 'admin/config/content/pressrelease/groups/delete/'. $result->group_id),
     		);
  	}
		// Build the table theme
		$html = theme('table', 
			array(
				'header' => $header,
				'rows' => $rows,
				'sticky' => TRUE,
				'empty' => t('No groups created.'),
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
 * Submit handler for the bundle configuration form.
 */
function pressrelease_groups_list_form_submit($form, &$form_state, $group_id = '') {
  $form_values = $form_state['values'];

  if (intval($form_state['values']['nodes'])) {
    variable_set('pressrelease_nodetypes', $form_state['values']['nodes']);
	}
  drupal_set_message(t('Your configuration has been saved.'));
}




/**
 * Form to create a Group.
 */
function pressrelease_groups_create_form($form, $form_state, $group_id = '') {
	// Buid the form
	$form['intro'] = array('#markup' => t('Create/Edit Email Group'));

  $form['values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Newsletter Group Details'),
    '#collapsible' => FALSE,
  );

	// Help for people who are lost
	if(is_numeric($group_id)) {
		// Get the default values of the form
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for update
		$form['values']['update'] = array(
			'#type' => 'hidden',
			'#value' => 1,
			);
		$form['values']['group_id'] = array(
			'#type' => 'hidden',
			'#value' => $result['group_id'],
			);
			$result['disable_roles'] = unserialize($result['disable_roles']);
			$result['disable_content'] = unserialize($result['disable_content']);
	} else {
		$result['title'] = '';
		$result['description'] = '';
		$result['disable_roles'] = array();
		$result['disable_content'] = array();
		$result['default_template'] = '';
	}

	$form['values']['title'] = array(
		'#type' => 'textfield',
		'#title' => t('Group Title'),
		'#default_value' => $result['title'],
		'#size' => 60,
		'#required' => TRUE,
		'#description' => t('Create a title for this group.'),
	);

	$form['values']['description'] = array(
		'#title' => t('Description'),
		'#type' => 'textarea',
		'#default_value' => $result['description'],
		//'#description' => t ('Email addresses must be seperated by a comma.  Addresses will be validated at the time the email is sent.'),
		//'#default_value' => isset( $context['keywords']) ? drupal_implode_tags($context['keywords']) : '',
		//'#required' => FALSE,
	);

	// Build the default template table
  $form['default'] = array(
    '#type' => 'fieldset',
    '#title' => t('Select default template'),
    '#collapsible' => TRUE,
  );

	$header = array(
		array('data' => 'Preview'),
		array('data' => 'Title', 'field' => 'title'),
		array('data' => 'Description'),
		);
	// Get all of the items for the row
	$query = db_select('pressrelease_template', 'p')
		->condition('deleted', 0)
		->extend('PagerDefault')
		//->limit(1)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('p', array (
			'template_id',
			'title',
			'description',
			'thumbnail_fid',
			));
	$template_results = $query->execute();

	// Create the table rows
  $rows = array();
  foreach($template_results as $template_result) {
		if($template_result->thumbnail_fid) {
		// display the image thumbnail
		$f = file_load($template_result->thumbnail_fid);
		$url = theme("image_formatter", array("image_style" => "thumbnail", "item" => array("uri" => $f->uri, "title" => $f->filename, "alt" => $template_result->title)));
		} else {
		$url = '';
		}
		//kpr($url);
		// now build the rows for the table
    $rows[$template_result->template_id] = array(
				$url,
        l($template_result->title, 'admin/config/content/pressrelease/templates/edit/'. $template_result->template_id),
        text_summary($template_result->description, 150),
     		);
  	}
		
/*
		// Build the table theme
		$html = theme('table', 
			array(
				'header' => $header,
				'rows' => $rows,
				'sticky' => TRUE,
				'empty' => t('No templates created.'),
			)
		);

	$form['default']['template']['pager'] = array(
		'#value' => theme('pager', array(
			'tags' => array()
		))
	);
*/
	$add_link = l('Add a template.', 'admin/config/content/pressrelease/templates/create');
  $form['default']['template'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
		'#default_value' => $result['default_template'],
    '#empty' => t('No templates found. !add_link', array('!add_link' => $add_link)),
		'#multiple' => FALSE,
  );


	// Check to see if access to roles is enabled.
	if(pressrelease_roles_access_check() == 1) {
		$form['roles'] = array(
			'#type' => 'fieldset',
			'#title' => t('Exclude this group from roles'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);
		$form['roles']['role_list']['#tree'] = TRUE;
		$form['roles']['role_list'] = array(
			'#type' => 'checkboxes',
			'#options' => group_get_role_types(variable_get('pressrelease_roles', array())),
			'#default_value' => $result['disable_roles'],
			'#description' => t('Please note that if a User is assigned two roles, one of which is excluded while the other is included, you will still be able to add that user to the group.'),
		);
	}

	// Check to see if the email module is present and that access to content is enabled.
	if(module_exists('email') && pressrelease_content_access_check() == 1) {
		$form['types'] = array(
			'#type' => 'fieldset',
			'#title' => t('Exclude this group from content types'),
			'#collapsible' => TRUE,
			'#collapsed' => TRUE,
		);
		$form['types']['content_list']['#tree'] = TRUE;
		$form['types']['content_list'] = array(
			'#type' => 'checkboxes',
			'#options' => group_get_content_types(variable_get('pressrelease_content', array())),
			'#default_value' => $result['disable_content'],
			'#description' => t('Exclude content from these entity types to be added to this email group. Remember that this list will only show content types with an Email field.'),
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

  return $form;	
}




/**
 * Submit handler for the group creation or update.
 */
function pressrelease_groups_create_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');
	//drupal_set_message('<pre>form_values:<br/>'.print_r($form_values, TRUE).'</pre>');

	$role_list = serialize($form_values['role_list']);
	$content_list = serialize($form_values['content_list']);

	// Check to see if this press release is being updated, act accordingly.
	if(!isset($form_values['update'])) {
	// Insert data
	db_insert('pressrelease_group')
		->fields(array(
			'title' => $form_values['title'],
			'description' => $form_values['description'],
			'modified' => $timestamp,
			'created' => $timestamp,
			'disable_roles' => $role_list,
			'disable_content' => $content_list,
			'default_template' => $form_values['template'],
		))
		->execute();
		drupal_set_message('Press Release Group was created.');
	} else {
			// or update
			$update = db_update('pressrelease_group')
			->fields(array(
				'title' => $form_values['title'],
				'description' => $form_values['description'],
				'modified' => $timestamp,
				'disable_roles' => $role_list,
				'disable_content' => $content_list,
				'default_template' => $form_values['template'],
				))
			->condition('group_id', $form_values['group_id'], '=')
			->execute();
		drupal_set_message('Press Release Group was updated.');
	}

/*
  // Now you can set the status
	if($form_values['thumbnail_upload']) {
		// Save the file
		$file = file_load($form_values['thumbnail_upload']);
		$file->status = FILE_STATUS_PERMANENT;
		file_save($file);
		file_usage_add($file, 'pressrelease', 'preview', $form_values['thumbnail_upload']);
		}
*/

	// Send them to the list.
  $form_state['redirect'] = array('admin/config/content/pressrelease/groups');
}

/**
 * Form for the Press Release Group delete confirmation.
 */
function pressrelease_groups_delete_form() {
	// Get our ID
	$group_id = arg(6);

	// Help for people who are lost; if no id send them home
	if(is_numeric($group_id)) {
		// Get the name to press release
		$result = db_select('pressrelease_group', 'g')
			->fields('g')
			->condition('group_id', $group_id, '=')
			->execute()
			->fetchAssoc();
	} else {
		drupal_goto('admin/config/content/pressrelease/groups');
	}
	// Build the form
  $form['intro'] = array(
		'#type' => 'item',
		'#title' => t('Please confirm that you want to delete this group: @title', array('@title' => $result['title'])),
		'#markup' => t('Warning: There is no undo for this action!'),
	);
	$form['values']['group_id'] = array(
		'#type' => 'hidden',
		'#value' => $group_id,
	);
  $form['values']['submit'] = array('#type' => 'submit', '#value' => t('Confirm Delete'));
	return $form;
}

/**
 * Submit handler for the template confirm delete.
 */
function pressrelease_groups_delete_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');
	// Delete the template
	$num_deleted = db_delete('pressrelease_group')
		->condition('group_id', $form_values['group_id'])
		->execute();
	// Let the user know it's gone
	drupal_set_message('Press Release Group was deleted.');
	// Send them to the list page
  $form_state['redirect'] = array(
    'admin/config/content/pressrelease/groups',
    //array('query' => array('key' => 'value')),
  );
}











/*
function pressrelease_autocomplete_user($string) {
$matches = array();
$return = db_query("SELECT name FROM users where lower(name) like '%".$string."%' LIMIT 10");
  // add matches to $matches 
  foreach ($return as $row) {
    $matches[$row->name] = check_plain($row->name);
  }
drupal_set_message('<pre>'.print_r($matches, TRUE).'</pre>');
  // return for JS
  drupal_json_output($matches);
}
*/


/**
 * Form to confirm role delete operation.
 */
/*
function user_admin_role_delete_confirm($form, &$form_state, $role) {
  $form['rid'] = array(
    '#type' => 'value',
    '#value' => $role->rid,
  );
  return confirm_form($form, t('Are you sure you want to delete the role %name ?', array('%name' => $role->name)), 'admin/people/permissions/roles', t('This action cannot be undone.'), t('Delete'));
}
*/