<?php
/**
 * Display form for Templates
 */
function pressrelease_template_list() {
	$header = array(
		array('data' => 'Preview'),
		array('data' => 'Title', 'field' => 'title'),
		array('data' => 'Description'),
		array('data' => 'Created', 'field' => 'created', 'sort' => 'asc'),
		array('data' => 'Edit'),
		array('data' => 'Delete'),
		);
	// Get all of the items for the row
	$query = db_select('pressrelease_template', 'p')
		->condition('deleted', 0)
		->extend('PagerDefault')
		->limit(10)
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('p', array (
			'template_id',
			'title',
			'description',
			'created',
			'modified',
			'deleted',
			'thumbnail_fid',
			'html',
			'css',
			));
	$results = $query->execute();
	// Create the table rows
  $rows = array();
  foreach($results as $result) {
		if($result->thumbnail_fid) {
		// display the image thumbnail
		$f = file_load($result->thumbnail_fid);
		$url = theme("image_formatter", array("image_style" => "thumbnail", "item" => array("uri" => $f->uri, "title" => $f->filename, "alt" => $result->title)));
		} else {
		$url = '';
		}
		//kpr($url);
		// now build the rows for the table
    $rows[] = array(
				$url,
        l($result->title, 'admin/config/content/pressrelease/templates/edit/'. $result->template_id),
        //truncate_utf8($result->description, 200, TRUE, TRUE),
        text_summary($result->description, 150),
        format_date($result->modified),
        l('Edit', 'admin/config/content/pressrelease/templates/edit/'. $result->template_id),
        l('Delete', 'admin/config/content/pressrelease/templates/delete/'. $result->template_id),
     		);
  	}
		// Build the table theme
		$html = theme('table', 
			array(
				'header' => $header,
				'rows' => $rows,
				'sticky' => TRUE,
				'empty' => t('No templates created.'),
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
 * Form for the Newsletter Template creation.
 */
function pressrelease_templates_create_form($form, $form_state, $template_id = '') {
	// Show the form header
  $form['intro'] = array(
		'#type' => 'item',
		'#title' => t('Create an Email Template here.'),
		'#markup' => t('The HTML wrapper can be pasted in this area. Content will be placed into the area with the <strong>*!CONTENT!*</strong> tag.'),
	);
  $form['values'] = array(
    '#type' => 'fieldset',
    '#title' => t('Create Newsletter Template'),
    '#collapsible' => FALSE,
  );

	// Help for people who are lost
	if(is_numeric($template_id)) {
		//drupal_goto('admin/config/content/pressrelease/templates');
		// Get the default values of the form
		$result = db_select('pressrelease_template', 'p')
			->fields('p')
			->condition('template_id', $template_id, '=')
			->execute()
			->fetchAssoc();
		// Add hidden form values for update
		$form['values']['update'] = array(
			'#type' => 'hidden',
			'#value' => 1,
			);
		$form['values']['template_id'] = array(
			'#type' => 'hidden',
			'#value' => $template_id,
			);
	} else {
		$result['title'] = '';
		$result['thumbnail_fid'] = '';
		$result['description'] = '';
		$result['html'] = '';
		$result['css'] = '';
		//$result['format'] = 'Full HTML';
	}

//dsm($result);

	// Build the form
	$form['values']['title'] = array(
		'#type' => 'textfield',
		'#title' => t('Template Title'),
		//'#default_value' => t('foo'),
		'#size' => 60,
		'#required' => TRUE,
		'#description' => t('Create a title for this Template.'),
		'#default_value' => $result['title'],
	);
  $form['values']['thumbnail_upload'] = array(
		'#name' => 'files[thumbnail_upload]',
	  '#type' => 'managed_file',
		'#title' => t('Upload Thumbnail'),
		'#size' => 48, 
		'#description' => t('If you have a preview image of this Template upload it here. Images only (png, gif, jpg, jpeg).'),
		'#upload_location' => 'public://pressrelease/',
		'#upload_validators' => array(
		  'file_validate_is_image' => array(),
    	'file_validate_extensions' => array('png gif jpg jpeg'),
			),
		'#default_value' => $result['thumbnail_fid'],
	);
	$form['values']['description'] = array(
		'#title' => t('Description'),
		'#type' => 'textarea',
		'#description' => t('Describe what this Template is.'),
		'#required' => FALSE,
		'#default_value' => $result['description'],
	);
	/*
	$form['values']['html'] = array(
		'#title' => t('HTML Template'),
		'#type' => 'textarea',
		'#rows' => 20,
		'#description' => t ('Please enter your HTML Template here.'),
		//'#default_value' => isset( $context['keywords']) ? drupal_implode_tags($context['keywords']) : '',
		'#required' => TRUE,
		'#default_value' => $result['html'],
	);
*/


	$form['values']['css'] = array(
    '#type' => 'textarea',
		'#title' => t('CSS Styles'),
		'#rows' => 15,
		'#description' => t ('Please enter your styles here.  They will be automatically placed inline with the HTML to help ensure that the email recipients see what you see here. Styles already placed inline will be preserved.  All other styles may be ignored by the recipient\'s email client.'),
		'#default_value' => $result['css'],
  );

	
	$form['values']['html'] = array(
    '#type' => 'text_format',
		'#title' => t('HTML Template'),
		'#description' => t ('Please enter your HTML Template here. Remember: content will be placed into the area with the <strong>*!CONTENT!*</strong> tag.'),
		'#default_value' => $result['html'],
    //'#format' => $result['format'],
		'#required' => TRUE,
  );
/*
	$result['format'] = 'full html';
		if (module_exists('token')) {
			$form['values']['tokens'] = array(
				'#theme' => 'token_tree',
				'#token_types' => array('all'),
			);
		}
		else {
			$form['values']['token_tree'] = array(
				'#markup' => '<p>' . t('Enable the <a href="@drupal-token">Token module</a> to view the available token browser.', array('@drupal-token' => 'http://drupal.org/project/token')) . '</p>',
			);
		}
*/
  $form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Save')
		);

	$form['cancel'] = array(
		'#type' => 'button',
		'#value' => t('Cancel'),
		'#executes_submit_callback' => TRUE,
		'#submit' => array('pressrelease_form_cancel_templates'),
		);

	return $form;
}

/**
 * Submit handler for the template creation or update.
 */
function pressrelease_templates_create_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
	//kpr($form_values);
	$timestamp = strtotime('now');
	//drupal_set_message('<pre>result:<br/>'.print_r($form_values, TRUE).'</pre>');
	// Check to see if this press release is being updated, act accordingly.
	if(!isset($form_values['update'])) {
	// Insert data
	db_insert('pressrelease_template')
		->fields(array(
			'title' => $form_values['title'],
			'description' => $form_values['description'],
			'html' => $form_values['html']['value'],
			'css' => $form_values['css'],
			'thumbnail_fid' => $form_values['thumbnail_upload'],
			'modified' => $timestamp,
			'created' => $timestamp
		))
		->execute();
		drupal_set_message('Press Release Template was created.');
	} else {
			// or update
			$update = db_update('pressrelease_template')
			->fields(array(
				'title' => $form_values['title'],
				'description' => $form_values['description'],
				'html' => $form_values['html']['value'],
				'css' => $form_values['css'],
				'thumbnail_fid' => $form_values['thumbnail_upload'],
				'modified' => $timestamp,
				))
			->condition('template_id', $form_values['template_id'], '=')
			->execute();
		drupal_set_message('Press Release Template was updated.');
	}

  // Now you can set the status
	if($form_values['thumbnail_upload']) {
		// Save the file
		$file = file_load($form_values['thumbnail_upload']);
		$file->status = FILE_STATUS_PERMANENT;
		file_save($file);
		file_usage_add($file, 'pressrelease', 'preview', $form_values['thumbnail_upload']);
		}
	// Send them to the list.
  $form_state['redirect'] = array(
    'admin/config/content/pressrelease/templates',
    //array('query' => array('key' => 'value')),
  );
}

/**
 * Form for the Newsletter Template delete confirmation.
 */
function pressrelease_templates_delete_form() {
	// Get our ID
	$template_id = arg(6);

	// Help for people who are lost; if no id send them home
	if(is_numeric($template_id)) {
		// Get the name to press release
		$result = db_select('pressrelease_template', 'p')
			->fields('p')
			->condition('template_id', $template_id, '=')
			->execute()
			->fetchAssoc();
	} else {
		drupal_goto('admin/config/content/pressrelease/templates');
	}
	// Build the form
  $form['intro'] = array(
		'#type' => 'item',
		'#title' => t('Please confirm that you want to delete this template: @title', array('@title' => $result['title'])),
		'#markup' => t('Warning: There is no undo for this action!'),
	);
	$form['values']['template_id'] = array(
		'#type' => 'hidden',
		'#value' => $template_id,
	);
  $form['values']['submit'] = array('#type' => 'submit', '#value' => t('Confirm Delete'));
	return $form;
}

/**
 * Submit handler for the template confirm delete.
 */
function pressrelease_templates_delete_form_submit($form, &$form_state) {
  $form_values = $form_state['values'];
	$timestamp = strtotime('now');
	// Delete the template
	$num_deleted = db_delete('pressrelease_template')
		->condition('template_id', $form_values['template_id'])
		->execute();
	// Let the user know it's gone
	drupal_set_message('Press Release Template was deleted.');
	// Send them to the list page
  $form_state['redirect'] = array(
    'admin/config/content/pressrelease/templates',
    //array('query' => array('key' => 'value')),
  );
}