<?php


// @todo: form handlers, update, delete - the basics
// @todo: connect users, connect 'user content type'.  Create user content type programatically? Probably not. Yikes, what to do here...

/*
function pressrelease_send() {
  return drupal_get_form('pressrelease_send_form');
}
*/
function pressrelease_send_form_page_four($form, &$form_state) {
	//kpr($form_state);
	$values = $form_state['storage'];
	//drupal_set_message('<pre>'.print_r($values, TRUE).'</pre>');

	// Deliver the press releases
	//$collection = pressrelease_send_prepare($form, $form_state);
	$collection = pressrelease_collect($form_state['storage']['group_list'], $form_state['storage']['addresses']);
	//kpr($collection);
	if(isset($collection)){
		// Call the function to send the message and show the results.		
		$results = pressrelease_send_deliver($collection, $values);	

		if(isset($results)) {
			// Delete the file and remove the temp variable containing the fid
			//$file = file_load($values['fid']);
			//if(file_delete($file)) {
				//variable_del('pressrelease_fid_temp_' . $nid, array());
				// Let the user know that the test email was sent
				drupal_set_message(t('Email was sent to :delivered addresses.  There were :errors errors.', array(':delivered' => $results['delivered'], ':errors' => $results['errors'])));
			//}
		}
	}

  $form['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
		//'#limit_validation_errors' => TRUE,
  );
	return $form;
}

// Page THREE
function pressrelease_send_form_page_three($form, &$form_state) {
	//kpr($form_state['storage']);
	//dsm('page three');
	// This is where we select the groups, additional addresses and template
	$form = pressrelease_send_preview($form, $form_state);
	return $form;
}
function pressrelease_send_form_page_three_submit($form, &$form_state) {
	//dsm('submit three');
  // Trigger multistep, there are more steps.
  $form_state['rebuild'] = TRUE;
  $values = $form_state['values'];
  if (isset($values['back']) && $values['op'] == $values['back']) {
    // User is moving back from this form, clear our storage.
		$form_state['storage']['fid'] = NULL;
  } else {
    // Set the next step.
    $form_state['storage']['step'] = 'pressrelease_send_form_page_four';
	}
}

// Page TWO
function pressrelease_send_form_page_two($form, &$form_state) {
	//kpr($form_state['storage']);
	//dsm('page two');
	// This is where we select the groups, additional addresses and template
	$form = pressrelease_send_select($form, $form_state);
	return $form;
}
function pressrelease_send_form_page_two_submit($form, &$form_state) {
  // Trigger multistep, there are more steps.
  $form_state['rebuild'] = TRUE;
  $values = $form_state['values'];
  if (isset($values['back']) && $values['op'] == $values['back']) {
    // User is moving back from this form, clear our storage.
		$form_state['storage']['template'] = NULL;
		$form_state['storage']['group_list'] = NULL;
		$form_state['storage']['addresses'] = NULL;
  } else {
    $form_state['storage']['group_list'] = $values['group_list'];
    $form_state['storage']['template'] = $values['template'];
    $form_state['storage']['addresses'] = $values['addresses'];
    $form_state['storage']['save_addresses'] = $values['save_addresses'];
    // Set the next step.
    $form_state['storage']['step'] = 'pressrelease_send_form_page_three';
	}
}

// Page ONE
function pressrelease_send_form_page_one($form, &$form_state) {
	//kpr($form_state['storage']);
	//dsm('page one');
	// Here's the form for the node selection:
	$form = pressrelease_send_dates_select($form, $form_state);
	return $form;
}
function pressrelease_send_form_page_one_submit($form, &$form_state) {
	//dsm('submit one');
  // Trigger multistep, there are more steps.
  $form_state['rebuild'] = TRUE;
  $values = $form_state['values'];
  if (isset($values['back']) && $values['op'] == $values['back']) {
    // User is moving back from this form, clear our storage.
		$form_state['storage']['nodes'] = NULL;
  } else {
	//kpr($values);
		foreach($values as $key => $input) {
			// Create the array of nodes
			if(is_array($input)){
				foreach($input as $nid){
					if(!empty($nid)){
						$nodes[$nid] = node_load($nid);
					}
				}
			}
		}
    $form_state['storage']['nodes'] = $nodes;
    $form_state['storage']['content_type'] = $values['content_type'];
    $form_state['storage']['group_by'] = $values['group_by'];
    $form_state['storage']['range'] = $values['range'];
    $form_state['storage']['begin'] = $values['begin'];
    $form_state['storage']['end'] = $values['end'];
    // Set the next step.
    $form_state['storage']['step'] = 'pressrelease_send_form_page_two';
	}
}

// Primary form builder.
function pressrelease_send_form($form, &$form_state) {
  // Initialize.
  if ($form_state['rebuild']) {
    // Don't hang on to submitted data in form state input.
    $form_state['input'] = array();
  }
  if (empty($form_state['storage'])) {
		
		// Determin what to do based the page source
		if(!is_numeric(arg(1)) && arg(4) != 'view'){
			 // No step has been set so start with the first.
			$form_state['storage'] = array(
				'step' => 'pressrelease_send_form_page_one',
			);
			$form_state['storage']['type'] = 'group';

		} elseif(arg(4) == 'view') {
			 // Skip the first step for this version.
			$form_state['storage'] = array(
				'step' => 'pressrelease_send_form_page_two',
			);
			// If it's a view style send then mark it so
			$form_state['storage']['type'] = 'view';
			$form_state['storage']['view_name'] = arg(5);
			$form_state['storage']['page_name'] = arg(6);
			$form_state['storage']['filter'] = arg(7);
		} elseif(is_numeric(arg(1))){
			 // Skip the first step for this version.
			$form_state['storage'] = array(
				'step' => 'pressrelease_send_form_page_two',
			);
			$nodes = node_load(arg(1));
			$form_state['storage']['nodes'] = array($nodes);
			$form_state['storage']['type'] = 'single';
		}
  }

  // Return the form for the current step.
  $function = $form_state['storage']['step'];
  $form = $function($form, $form_state);
  return $form;
}
// Primary submit handler.
function pressrelease_send_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  // Check if we're moving back or forward in the form.
  if (isset($values['back']) && $values['op'] == $values['back']) {
    // Moving back in form.
    $step = $form_state['storage']['step'];
    // Call current step submit handler if it exists to unset step form data.
    if (function_exists($step . '_submit')) {
      $function = $step . '_submit';
      $function($form, $form_state);
    }
    // Remove the last saved step so we use it next.
    $last_step = array_pop($form_state['storage']['steps']);
    $form_state['storage']['step'] = $last_step;
  }
  else {
    // Record the current step.
    $step = $form_state['storage']['step'];
    $form_state['storage']['steps'][] = $step;
    // Call step submit handler if it exists.
    if (function_exists($step . '_submit')) {
      $function = $step . '_submit';
      // Current step's submit handler will set the next step.
      $function($form, $form_state);
    }
  }
  return;
}



function pressrelease_send_record($eid_array, $user, $recipients) {
	//drupal_set_message('<pre>eid_array: '.print_r($eid_array, TRUE).'</pre><hr />');	
	
	foreach($eid_array as $eid){
		//drupal_set_message('eid: '.$eid);
		// First check to see if the node has already been recorded as 'sent'
	
		$result = db_select('pressrelease_record', 'p')
			->fields('p')
			->condition('eid', $eid)
			->execute()
			->fetchField();
	
		//drupal_set_message('result: '.$result);
		if(!$result) {
			// insert the send record
			$eid_new = db_insert('pressrelease_record')
			->fields(array(
				'eid' => $eid,
				'uid' => $user->uid,
				'date_sent' => strtotime('now'),
				'send_started' => strtotime('now'),
				'send_complete' => strtotime('now'),
				'total_recipients' => $recipients,
				))
			->execute();
		} else {
			// If the release has already been sent then udpate the sent time
			$eid_update = db_update('pressrelease_record')
				->fields(array(
					'uid' => $user->uid,
					'date_sent' => strtotime('now'),
					'send_complete' => strtotime('now'),
					'total_recipients' => $recipients,
				))
				->condition('eid', $eid, '=')
				->execute();
		}
	}
}


/**
 * Send the email
 */
function pressrelease_send_deliver($collection, $values){

	//drupal_set_message('<pre>values: '.print_r($values, TRUE).'</pre><hr />');	
	//drupal_set_message('<pre>collection: '.print_r($collection, TRUE).'</pre><hr />');
	
/*	//$nid = arg(1);
	//$fid = 	variable_get('pressrelease_fid_temp_' . $nid, array());
	if($html_pressrelease_load = file_load($values['fid'])){
kpr($html_pressrelease_load);
dsm('file loaded');
	} else {
dsm('file NOT loaded');
	}
	//$html_pressrelease_display = file_create_url($html_pressrelease_load->uri);


	$html_pressrelease_display = file_create_url($html_pressrelease_location);
	$pattern = '/\/\/sites/'; // to fix the extra forward slash that's magically appearing in the URL
	if(preg_match($pattern, $html_pressrelease_display)){
		$html_pressrelease_display = preg_replace($pattern, '/sites', $html_pressrelease_display);
	}
*/

	//$request = drupal_http_request($html_pressrelease_display);
	$request = drupal_http_request($values['file']);
	$content = $request->data;

	$results = array();
	$results['delivered'] = 0;
	$results['errors'] = 0;
	$module = 'pressrelease';
	$key = 'delivery';
	$language = language_default();
	$params = array();
	$params['from'] = trim(mime_header_encode(variable_get('site_mail', '')));
	//$params['headers']['Reply-To'] = trim(mime_header_encode(variable_get('site_mail', '')));
	$params['headers']['Reply-To'] = $user->mail;
  
	// Create a subject for the email.
	global $base_url;
	$subject = 'Message from '. $base_url . ' - ' . date('l M j, Y', REQUEST_TIME);

	global $user;
	$from = $user->mail;
	//$from = 'no-reply@kued.org';	
	$send = FALSE;

	// Send the messages
	$recipients = 0;

	// insert the primary send record
	$send_record = db_insert('pressrelease_send_record')
	->fields(array(
		'date_sent' => strtotime('now'),
		'send_started' => strtotime('now'),
		'send_complete' => strtotime('now'),
		'total_recipients' => $recipients,
		'file_url' => $values['fid'],
		))
	->execute();
	//drupal_set_message('send_record: '.$send_record);
/*
	$record_one = db_select('pressrelease_send_record', 'p')
		->fields('p')
		->condition('record_id', $send_record)
		->execute()
		->fetchAll();
	drupal_set_message('send_record data 1: <pre>'.print_r($record_one, TRUE).'</pre>');
*/

	// Now send the emails
	foreach($collection as $address){
		$email = check_plain(trim($address));
		$message = drupal_mail($module, $key, $email, $language, $params, $from, $send);
			$message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
			$message['subject'] = t($subject);
			$message['body'] = $content;

		// Retrieve the responsible implementation for this message.
		$system = drupal_mail_system($module, $key);

		// Send e-mail.
		if($message['result'] = $system->mail($message)) {
			$results['delivered']++;
		} else {
			$results['errors']++;
		}
	$recipients++;
	}

	// Build collection of nids to record the send
	foreach($values['nodes'] as $eid => $node){
		$eid_array[] = $eid;
	}
	// Record the individual node sent data
	pressrelease_send_record($eid_array, $user, $recipients);


	// Update the completed time and number of recipients
	$send_record_update = db_update('pressrelease_send_record')
		->fields(array(
			'send_complete' => strtotime('now'),
			'total_recipients' => $recipients,
		))
		->condition('record_id', $send_record, '=')
		->execute();

/*
		$record_two = db_select('pressrelease_send_record', 'p')
			->fields('p')
			->condition('record_id', $send_record)
			->execute()
			->fetchAll();
		drupal_set_message('send_record data 2: <pre>'.print_r($record_two, TRUE).'</pre>');
*/

return $results;

/*
$batch = array(
  'title' => t('Sending Group Email'),
  'init_message' => t('Starting to send messages'),
	'finished' => '',
  'progress_message' => t('Sent @current out of @total'),
  'error_message' => t('An error occurred and some or all of the exports have failed.'),

  'finished' => '_pressrelease_deliver_complete',
  //'file' => 'path_to_file_containing_myfunctions',
);
$batch['operations'][] = array('pressrelease_deliver_single_process', $collection);

batch_set($batch);

batch_process();
*/

}


/**
 * Page two: Press Release Preview
 * Builds the preview as well as the HTML document that is sent and saved for archive
 */
function pressrelease_send_preview(&$form, &$form_state){
	module_load_include('inc', 'pressrelease', 'pressrelease.send');
	$values = $form_state['storage'];

	// Count connected users
	$query = db_select('pressrelease_template', 't')
		->condition ('template_id', $values['template'])
		->fields ('t', array (
			'template_id',
			'title',
			'description',
			'modified',
			'thumbnail_fid',
			'html',
			'css'
			))
		->execute();
		$template = $query->fetchAssoc();

	// Switch for each source type	
	switch ($form_state['storage']['type']) {
		case 'single':
			//dpm('single preview');
			// Get the single Node
			$nid = arg(1);
			$node = node_load($nid);
      $title = $nid;
			$node_view = node_view($node, 'pressrelease');
			$node_view['links']['#access'] = FALSE;
			$node_view['#contextual_links'] = FALSE;
			$node_render = drupal_render($node_view);
			$embed = pressrelease_fix_urls($node_render);
			//kpr($node_render);
		break;
		case 'group':
			//dpm('group preview');
			// Get group of Nodes
			$nodes = $values['nodes'];
			$embed = '';
				foreach($nodes as $item) {
					$nid = $item->nid;
					$title = $item->nid;
					$node = node_load($nid);
					$node_view = node_view($node, 'pressrelease_tease');
					$node_view['links']['#access'] = FALSE;
					$node_view['#contextual_links'] = FALSE;
					$node_render = drupal_render($node_view);
					$embed .= pressrelease_fix_urls($node_render);
					$embed .= '<hr />';
			}
		break;
		case 'view':
			//dpm('view preview');
			// Else embed the view
			$embed = views_embed_view($form_state['storage']['view_name'], $form_state['storage']['page_name'], $form_state['storage']['filter']);
			//dsm('view_embed'.$view_embed);
			//kpr($view_embed);
			$title = $form_state['storage']['view_name'] .'_'. $form_state['storage']['page_name'] .'_'. $form_state['storage']['filter'];
		break;
	}

	// Replace the content chunk
	$template_prepare = preg_replace('/\*\|CONTENT\|\*/', $embed, $template['html']);
  // Maybe something loaded the class without telling libraries API.
	if(!empty($template['css'])){
    if(!class_exists('\Pelago\Emogrifier')) {
      $library = libraries_get_path('emogrifier') . '/Classes/Emogrifier.php';
      if(include_once "$library") {
        //dpm($template_prepare);
        //dpm($template['css']);
        $emogrifier_present = TRUE;
        $emogrifier = new \Pelago\Emogrifier($template_prepare, $template['css']);
        $template_prepare = $emogrifier->emogrify();
        //dpm($template_prepare);
      }
    }
  }

	// Prepare a temp file to display and later send
	$html_pressrelease_directory = 'public://pressrelease/send/';
	// Transliterate the filename
	if(function_exists('transliteration_clean_filename')) {
		$html_pressrelease_name = transliteration_clean_filename('pr-' . $template['title'] . '-' . $title . '.htm');
		}
	// Make full filename
	$html_pressrelease_location = $html_pressrelease_directory . $html_pressrelease_name;

	$html_pressrelease_display = file_create_url($html_pressrelease_location);
	$pattern = '/\/\/sites/'; // to fix the extra forward slash that's magically appearing in the URL
	if(preg_match($pattern, $html_pressrelease_display)){
		$html_pressrelease_display = preg_replace($pattern, '/sites', $html_pressrelease_display);
	}
	//kpr($html_pressrelease_location);
	//kpr($html_pressrelease_display);
	
	// Tokens to replace eventually:
	//$tokensobject = token_get_values($type = 'node', $nid);
	/*
	*|CONTENT|*
	*|ARCHIVE|*
	*|MC:SUBJECT|*
	*|SUBJECT|*
	*|TEASE|*
	*|TWITTER:PROFILEURL|*
	*|FACEBOOK:PROFILEURL|*
	*|FORWARD|*
	*|UNSUB|*
	*|DATE|*
	
	*|CURRENT_YEAR|*
	*|LIST:COMPANY|*
	*|HTML:LIST_ADDRESS_HTML|*
	*/
	$site_name =  variable_get('site_name', 'Drupal');
	$template_display = preg_replace('/\*\|ARCHIVE\|\*/', $html_pressrelease_display, $template_prepare);
	$template_display = preg_replace('/\*\|SUBJECT\|\*/', $site_name . ' Press Release', $template_display);
	$template_display = preg_replace('/\*\|MC:SUBJECT\|\*/', $site_name . ' Press Release', $template_display);
	$template_display = preg_replace('/\*\|DATE\|\*/', date('l, F jS, Y', REQUEST_TIME), $template_display);

	// Create the iframe to temporarily display the newsletter - this file is removed when the message is sent
	if($full_location = file_prepare_directory($html_pressrelease_directory, $options = FILE_CREATE_DIRECTORY)){
		$fid = 	variable_get('pressrelease_fid_temp_' . $title, array());
		$fid_tracker = 'pressrelease_fid_temp_' . $title;

		// File management
		if(!$fid) {
				$html_pressrelease_save = file_save_data($template_display, $html_pressrelease_location);
				variable_set($fid_tracker, $html_pressrelease_save->fid);
			} else {
				// Always delete the old file and create a new one, ensures it's current
				$html_pressrelease_load = file_load($fid);
				$html_pressrelease_delete = file_delete($html_pressrelease_load);
				variable_del('pressrelease_fid_temp_' . $title);
				$html_pressrelease_save = file_save_data($template_display, $html_pressrelease_location);
				variable_set($fid_tracker, $html_pressrelease_save->fid);
			}
		$form_state['storage']['fid'] = $fid; // add the fid to storage, I'll need it.
		$form_state['storage']['fid_tracker'] = $fid_tracker;	
		$form_state['storage']['file'] = $html_pressrelease_display;

		// File display
		$html_pressrelease_iframe = '<iframe width="100%" height="600" frameborder="1" id="pressrelease-preview" src="' . $html_pressrelease_display . '"></iframe>';
		}

	$form['test'] = array(
		'#type' => 'fieldset',
		'#title' => t('Send Test Email (optional)'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
	);
	$form['test']['address'] = array(
		'#type' => 'textfield',
		'#title' => t('Address (one only)'),
	);
	$form['test']['location'] = array(
		'#type' => 'hidden',
		'#value' => $html_pressrelease_location,
	);
	$form['test']['nid'] = array(
		'#type' => 'hidden',
		'#value' => $title,
	);
  $form['test']['send_test'] = array(
    '#type' => 'button',
    '#value' => t('Send Test Email'),
		'#executes_submit_callback' => FALSE,
    '#submit' => array('pressrelease_send_form_test'),
		'#validate' => array('pressrelease_send_form_test_validate'),
  );
	$form['preview'] = array(
		'#type' => 'fieldset',
		'#title' => t('Preview the Press Release'),
		//'#description' => t('Separate addresses with a comma!'),
		'#collapsible' => FALSE,
		'#collapsed' => FALSE,
	);
	//kpr(variable_get('pressrelease_fid_temp_' . $title, array()));
	$form['preview']['template_info'] = array(
		'#type' => 'item',
		'#title' => t('Preview: ' . l($template['title'] . ' (open in new window)', $html_pressrelease_display, array('attributes' => array('target'=>'_blank')))),
		'#description' => $template['description'],
	);
	$form['preview']['template_preview'] = array(
		'#type' => 'markup',
		'#markup' => $html_pressrelease_iframe,
		//'#prefix' => '<div id="pressrelease-preview-container">',
		//'#suffix' => '</div>',
	);
	$form['buttons'] = array(
		'#type' => 'fieldset',
		'#title' => t('Deliver this release'),
		'#collapsible' => FALSE,
	);
  $form['buttons']['back'] = array(
    '#type' => 'submit',
    '#value' => t('Back'),
    //'#submit' => array('pressrelease_send_form_back'),
    '#limit_validation_errors' => array(),
  );
  $form['buttons']['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send Message'),
    //'#submit' => array('pressrelease_send_form_confirmed'),
    '#limit_validation_errors' => array(),
  );

/*
	$form['list'] = array(
		'#type' => 'fieldset',
		'#title' => t('List of email addresses who will receive this message'),
		//'#description' => t('Separate addresses with a comma!'),
		'#collapsible' => TRUE,
		'#collapsed' => TRUE,
	);
	$form['list']['email_list'] = array(
		'#type' => 'markup',
		'#markup' => 'List',
		//'#markup' => $node_render,
		//'#markup' => $template_display,
		//'#prefix' => '<div id="pressrelease-preview-container">',
		//'#suffix' => '</div>',
	);
*/
	return $form;
}

/**
 * Validate test email address at pressrelease_send_form_test().
 */
function pressrelease_send_form_test_validate($form, &$form_state) {
	$values = $form_state['values'];
	if (!valid_email_address($form_state['values']['address'])) {
		form_set_error('email', t('You must include a real email address!'));
  } else {
		pressrelease_send_form_test($values);
	}
}

/**
 * Submit handler for pressrelease_send_form_test() button
 */
//function pressrelease_send_form_test($form, &$form_state) {
function pressrelease_send_form_test($values) {
	$nid = $values['nid'];
	$fid = 	variable_get('pressrelease_fid_temp_' . $nid, array());

	$html_pressrelease_load = file_load($fid);
	$html_pressrelease_display = file_create_url($html_pressrelease_load->uri);

	$request = drupal_http_request($html_pressrelease_display);
	$content = $request->data;
	$site_name =  variable_get('site_name', 'Drupal');
	$module = 'pressrelease';
  $key = 'preview';
	$email = check_plain(trim($values['address']));
  $language = language_default();
	$params = array();
	$params['from'] = trim(mime_header_encode(variable_get('site_mail', '')));
	//$params['headers']['Reply-To'] = trim(mime_header_encode(variable_get('site_mail', '')));
  $params['headers']['Reply-To'] = $user->mail;
  $from = trim($values['address']);
  $send = FALSE;
  $message = drupal_mail($module, $key, $email, $language, $params, $from, $send);
	$message['headers']['Content-Type'] = 'text/html; charset=UTF-8; format=flowed';
	$message['subject'] = t('Test Press Release from ' . $site_name);
	$message['body'] = $content;

  // Retrieve the responsible implementation for this message.
  $system = drupal_mail_system($module, $key);

  // Send e-mail.
  $message['result'] = $system->mail($message);

	// Let the user know that the test email was sent
	drupal_set_message(t('Test email successfully sent.'));
}



function pressrelease_send_select($form, &$form_state) {
	$input = $form_state['storage'];
	$nodes = $input['nodes'];
	
	// Switch for each message type
	switch ($form_state['storage']['type']) {
		case 'single':
			// We'll just use the nid in the URL.
			// If this has already been sent we need to notify the user
			$result = db_select('pressrelease_record', 'p')
				->fields('p')
				->condition('eid', arg(1))
				->execute()
				->fetchAssoc();
				// If the node has been sent then let the user know
				if($result) {
					//dsm($result);
					$date_sent = format_date($result['date_sent'], 'long');
					$user = user_load($result['uid']);
					$content_type = ucwords($input['nodes'][0]->type);
					drupal_set_message(t('<strong><em>Please Note</em></strong>: This <em>@type</em> record was previously sent as a Press Release on <strong>@date_sent</strong> by !username', array('@date_sent' => $date_sent, '@type' =>$content_type, '!username' => theme('username', array('account' => $user)))), 'warning');
				}
		break;
		case 'group':
			$form['options'] = array(
				'#type' => 'fieldset',
				'#title' => t('Selected items for this release'),
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
			);
	
			// Build the table header.
			$header = array(
					'title' => t('Title'),
					'pressrelease_date' => t('Release Date'),
					'type' => t('Type'),
					'author' => t('Author'),
			);
			foreach($nodes as $node) {
				$pressrelease_date = date('m/d/Y', strtotime($node->field_pressrelease_dates['und'][0]['value']));
				$options[$node->nid] = array(
					'title' => array(
						'data' => array(
							'#type' => 'link',
							'#title' => $node->title,
							'#href' => 'node/' . $node->nid,
							//'#options' => array(),
							//'#options' => $l_options,
							//'#suffix' => ' ' . theme('mark', array('type' => node_mark($node->nid, $node->changed))),
						),
					),
					'type' => check_plain(node_type_get_name($node)),
					'author' => theme('username', array('account' => $node)),
					'pressrelease_date' => $pressrelease_date,
				);
			}
			if(empty($options)) {
				$options = array();
			}
			$form['options']['nodes'] = array(
				'#type' => 'tableselect',
				'#header' => $header,
				'#options' => $options,
				'#empty' => t('No nodes were selected.'),
				'#default_value' => $nodes,
			);
		break;
		
		case 'view':
			$form['options'] = array(
				'#type' => 'fieldset',
				'#title' => t('Selected view for this mailing'),
				'#collapsible' => TRUE,
				'#collapsed' => FALSE,
			);
			$form['options']['view_name'] = array(
				'#type' => 'textfield',
				'#title' => t('View name'),
				'#empty' => t('Missing the View Name!'),
				'#default_value' => $form_state['storage']['view_name'],
			);
			$form['options']['page_name'] = array(
				'#type' => 'textfield',
				'#title' => t('Page name'),
				'#empty' => t('Missing the Page Name!'),
				'#default_value' => $form_state['storage']['page_name'],
			);
			$form['options']['filter'] = array(
				'#type' => 'textfield',
				'#title' => t('View Filter'),
				'#empty' => t('No filter was found!'),
				'#default_value' => $form_state['storage']['filter'],
			);
		break;
	}

	// Get the node id
	$form['groups'] = array(
		'#type' => 'fieldset',
		'#title' => t('Assign groups'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);

	$header = array(
		array('data' => 'Title'),
		array('data' => 'Total Addresses'),
		array('data' => 'Description'),
		);
	// Get all of the items for the row
	$query = db_select('pressrelease_group', 'g')
		->extend('TableSort')
		->orderByHeader($header)
		->fields ('g', array (
			'group_id',
			'title',
			'description',
			));
	$results = $query->execute();
	// Create the table rows
  $rows = array();
  foreach($results as $result) {

		// Count connected email addresses
		$email_query = db_select('pressrelease_email_connect', 'g')
			->condition ('group_id', $result->group_id)
			->fields ('g', array (
				'email_connect_id',
				))
			->execute();
		$email_count = $email_query->rowCount();
	
		// Count connected users
		$user_query = db_select('pressrelease_user_connect', 'g')
			->condition ('group_id', $result->group_id)
			->fields ('g', array (
				'user_connect_id',
				))
			->execute();
		$user_count = $user_query->rowCount();

		// TODO: Need to add content stuff here
		$content_count = '0';

		// Build the totals display
		$total_count = $email_count + $user_count + $content_count;
		$total_count_display = '';
		if($email_count != 0){
			$total_count_display .= t('Email: ' . $email_count . '<br />');
		}
		if($user_count != 0){
			$total_count_display .= t('Users: ' . $user_count . '<br />');
		}
		if($content_count != 0){
			$total_count_display .= t('Content: ' . $content_count . '<br />');
		}
		if($total_count != 0){
			$total_count_display .= t('Total: ' . $total_count . '<br />');
		} else {
			$total_count_display = t('There are no email addresses associated with this group!');
		}

		// now build the rows for the table
    $rows[$result->group_id] = array(
			t($result->title),
      $total_count_display,
			text_summary($result->description, 100),
     	);
  	}

  $form['groups']['group_list'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#empty' => t('No groups found.'),
		'#multiple' => TRUE,
		'#required' => TRUE,
		'#title' => t('Assign Groups'),
    '#default_value' => !empty($form_state['storage']['group_list']) ? $form_state['storage']['group_list'] : array(),
  );

	$form['extra_addresses'] = array(
		'#type' => 'fieldset',
		'#title' => t('Add Additional addresses'),
		'#description' => t('Separate addresses with a comma!'),
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);

//dpm($form_state);

	$form['extra_addresses']['addresses'] = array(
		'#title' => t('Additional addresses'),
		'#type' => 'textarea',
		'#description' => t('Please note, the email will be sent to these addresses but they will not be stored.'),
    '#default_value' => !empty($form_state['storage']['addresses']) ? $form_state['storage']['addresses'] : '',
	);

	$form['extra_addresses']['save_addresses'] = array(
		'#title' => t('Save these addresses'),
		'#type' => 'checkbox',
		'#description' => t('If you would like to keep these addresses in the database check this box. To use them later you will need to add them to a group. <em>Please Note</em>: if this box is checked an address already exists in the database it will not be duplicated.'),
    '#default_value' => !empty($form_state['storage']['save_addresses']) ? $form_state['storage']['save_addresses'] : '',
	);

	$form['template'] = array(
		'#type' => 'fieldset',
		'#title' => t('Select template of all assigned groups'),
		'#description' => t('Choose a template.  Content will be placed into the area with the <strong>*!CONTENT!*</strong> tag in your template'),
		'#required' => TRUE,
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
	);

	$header = array(
		array('data' => 'Preview'),
		array('data' => 'Title', 'field' => 'title'),
		array('data' => 'Description'),
		);
	// Get all of the items for the row
	$query = db_select('pressrelease_template', 'p')
		->condition('deleted', 0)
		//->extend('PagerDefault')
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
		$thumbnail_url = theme("image_formatter", array("image_style" => "thumbnail", "item" => array("uri" => $f->uri, "title" => $f->filename, "alt" => $template_result->title)));
		} else {
		$thumbnail_url = '';
		}
		// now build the rows for the table
    $rows[$template_result->template_id] = array(
			$thumbnail_url,
      $template_result->title,
      //l($template_result->title, 'admin/config/content/pressrelease/templates/edit/'. $template_result->template_id),
      text_summary($template_result->description, 150),
     	);
  	}

	$add_link = l('Add a template.', 'admin/config/content/pressrelease/templates/create');
  $form['template']['template'] = array(
		'#title' => t('Template'),
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $rows,
    '#default_value' => !empty($form_state['storage']['template']) ? $form_state['storage']['template'] : array(),
    '#empty' => t('No templates found. !add_link', array('!add_link' => $add_link)),
		'#multiple' => FALSE,
  );

	// only the group type needs a back button...the others have nowhere to back to
	// other types start on the second step.
	if($form_state['storage']['type'] == 'group'){
		$form['buttons']['back'] = array(
			'#type' => 'submit',
			'#value' => t('Back'),
			'#limit_validation_errors' => array(),
		);
	}

	// submit the form
	$form['submit'] = array(
		'#type' => 'submit',
		'#value' => t('Preview before sending'),
		);

	return $form;
}


//////////////////////////
function pressrelease_send_validate($form, &$form_state) {
	if($form_state['input']['begin'] > $form_state['input']['end']) {
		form_set_error('begin', 'Your start date is later than your end date.  Please try again.');
	}
}

/**
 * Form for the Newsletter sending process.
 */
function pressrelease_send_dates_select($form, &$form_state) {

	//kpr($form_state);
  //$values = $form_state['values'];

  $admin_access = user_access('administer nodes');

	if(isset($form_state['values']['filter'])){
		$values = $form_state['values'];
	} elseif(!empty($form_state['values']['back'])) {
		$values = $form_state['storage'];
	} else {
		$values = array();
    $values['group_by'] = 'week';
    $values['content_type'] = '';
    $values['range'] = 16;
		$values['begin'] = '';
		$values['end'] = '';
		$values['submit'] = '';
	}

  $grouping = pressrelease_date_grouping($values['group_by'], $values['range']);
	//dsm($grouping);

	if(!$values['begin'] || !$values['end']) {
		$values['begin'] = $grouping['current']['begin'];
		$values['end'] = $grouping['current']['end'];
	}

  // Build my date range search arrays
  foreach($grouping as $key => $group){
    if(is_numeric($key)){
      $search_begin[$group['search']['begin']] = $group['display']['begin'];
      $search_end[$group['search']['end']] = $group['display']['end'];
      $search_week_num[] = $group['week_num'];
		}
  }

  // Get the assigned content types to start out with
  // We'll need this several times
  $content_types = variable_get('pressrelease_content_types', array());
  // Now narrow the list
  $content_active = array();
  foreach($content_types as $key => $type){
    if($type != '0'){
      $type_readable = $type;
      $type_readable = node_type_get_name($type_readable);
      $content_active[$type_readable] = $type;
      $content_active_list[$type] = $type_readable;
    }
  }

/*
  // Build the 'Update options' form.
  $form['options'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter results'),
    '#attributes' => array('class' => array('container-inline')),
    '#access' => $admin_access,
  );
*/
  $options = array();
  foreach (module_invoke_all('node_operations') as $operation => $array) {
    $options[$operation] = $array['label'];
  }

  $form['options']['content_type'] = array(
    '#type' => 'select',
    '#title' => t('Filter by Content Type'),
    //'#title_display' => 'invisible',
    '#options' => $content_active_list,
    //'#default_value' => '',
    '#empty_option' => t('- all -'),
    '#default_value' => $values['content_type'],
    //'#description' => t('Set this to <em>Yes</em> if you would like this category to be selected by default.'),
  );


  $date_groups = array(
    'chron' => t('Chronological'),
    //'day' => t('Day'),
    'week' => t('Week'),
    //'month' => t('Month'),
  );
  $form['options']['group_by'] = array(
    '#type' => 'select',
    '#title' => t('Group by Date'),
    '#options' => $date_groups,
    '#empty_option' => t('none'),
    '#default_value' => $values['group_by'],
    '#description' => t('<em>Please note</em>: This grouping uses the Press Release date setting. If the content does not have a Press Release date set it will not appear in this list.<br />'),
  );

  // Date range display
  $range_group = array(
    '4' => '4',
    '6' => '6',
    '8' => '8',
    '12' => '12',
    '16' => '16',
    '24' => '24',
    '36' => '36'
  );
  $form['options']['range'] = array(
    '#type' => 'select',
    '#title' => t('Range of weeks'),
    '#options' => $range_group,
    '#default_value' => $values['range'],
    '#description' => t('Select the range of weeks to work with. <em>Note:</em> I may not keep this...<br />'),
  );

  $form['options']['showing'] = array(
    '#type' => 'item',
    '#title' => t('<hr />Current week: @begin to @end', array('@begin' => $grouping['current']['begin_display'], '@end' => $grouping['current']['end_display'])),
  );

  // Show the display start date:
  $form['options']['begin'] = array(
    '#type' => 'select',
    '#title' => t('Show this date range:'),
    '#options' => $search_begin,
    '#default_value' => $values['begin'],
		'#empty_option' => $grouping['current']['begin'],
		//'#empty_value' => $values['begin'],
    //'#description' => t('Select the range of weeks to work with. <em>Note:</em> I may not keep this...<br />'),
  );

  // Show the display end date:
  $form['options']['end'] = array(
    '#type' => 'select',
    '#title' => t('Through:'),
    '#options' => $search_end,
    '#default_value' => $values['end'],
		'#empty_option' => $grouping['current']['end'],
		//'#empty_value' => $values['end'],
    //'#description' => t('Select the range of weeks to work with. <em>Note:</em> I may not keep this...<br />'),
  );

  $form['options']['submit'] = array(
    '#type' => 'button',
    '#value' => t('Filter'),
		'#executes_submit_callback' => FALSE,
		'#name' => 'filter',
    '#validate' => array('pressrelease_send_validate'),
    //'#submit' => array('pressrelease_send_form_page_two'),
  );

  //if (isset($form_state['input']['filter']['Filter'])) {
		$form['options']['select'] = array(
			'#type' => 'submit',
			'#value' => t('Next -> Select Groups'),
			//'#executes_submit_callback' => FALSE,
			'#name' => 'submit',
			'#validate' => array('pressrelease_send_validate'),
			//'#submit' => array('pressrelease_send_form_page_two'),
		);
  //}

	// Function to group dates into weeks
  $grouping = pressrelease_date_grouping($values['group_by'], $values['range']);
	if(!$values['begin'] || !$values['end']) {
		$values['begin'] = $grouping['current']['begin'];
		$values['end'] = $grouping['current']['end'];
	}

  // Build my date range search arrays
  foreach($grouping as $key => $group){
    if(is_numeric($key)){
      $search_begin[$group['search']['begin']] = $group['display']['begin'];
      $search_end[$group['search']['end']] = $group['display']['end'];
      $search_week_num[] = $group['week_num'];
		}
  }

  // Get the assigned content types to start out with
  // We'll need this several times
  $content_types = variable_get('pressrelease_content_types', array());
  // Now narrow the list
  $content_active = array();
  foreach($content_types as $key => $type){
    if($type != '0'){
      $type_readable = $type;
      $type_readable = node_type_get_name($type_readable);
      $content_active[$type_readable] = $type;
      $content_active_list[$type] = $type_readable;
    }
  }

  // Build the sortable table header.
  $header = array(
      'title' => t('Title'),
			'previously_sent' => t('Previously Sent On'),
			'pressrelease_date' => t('Release Date'),
			'type' => t('Type'),
			'author' => t('Author'),
			'status' => t('Status'),
			'changed' => t('Last Update'),
  );

	// Build the actual search array here.
	if(!empty($form_state['values'])){
		foreach($search_begin as $key => $value){
			if($values['begin'] <= $key && $values['end'] >= $key){
				$search_range_begin[$key] = $value;
			}
		}
		foreach($search_end as $key => $value){
			if($values['begin'] <= $key && $values['end'] >= $key){
				$search_range_end[$key] = $value;
			}
		}
	} else {
		$search_range_begin = $values['begin'];
		$search_range_end = $values['end'];
	}

	// One last process before I'm ready to built my node list
	$i=1;
	$count = count($search_range_begin);
	while($i <= $count){
		current($search_range_begin);
		$search_block[$i]['begin'] = date('Y-m-d H:i:s', key($search_range_begin));
		next($search_range_begin);

		current($search_range_end);
		$search_block[$i]['end'] = date('Y-m-d H:i:s', key($search_range_end));
		next($search_range_end);
		$i++;
	}

	$i=0;
	foreach($search_block as $search){
		//Build filtered EntityFieldQuery here
		$query = new EntityFieldQuery();
		$query->entityCondition('entity_type', 'node')
		->propertyCondition('status', 1)
		->fieldCondition('field_pressrelease_dates', 'value', array($search['begin'], $search['end']), 'BETWEEN')
		->fieldOrderBy('field_pressrelease_dates', 'value', 'ASC')
		->range(0, 50)
		->addMetaData('account', user_load(1)); // Run the query as user 1.

		// Filter content type
		if(!empty($values['content_type'])){
			$query->entityCondition('bundle', $values['content_type']);
		} else {
			$query->entityCondition('bundle', $content_active);
		}

		// Submit the field query
		$result = $query->execute();
		// If we get results then load up the nodes
		if (isset($result['node'])) {
			$items = array_keys($result['node']);
			$node_list[$i] = entity_load('node', $items);
			$node_list[$i]['week_begin'] = $search['begin'];
			$node_list[$i]['week_end'] = $search['end'];
		} else {
			$node_list[$i] = '';
			$node_list[$i]['week_begin'] = $search['begin'];
			$node_list[$i]['week_end'] = $search['end'];
		}
	$i++;
	$query = '';
	}

  $destination = drupal_get_destination();
  $options = array();
	$operations = array();
	$item = '';
	$i = 0;
	foreach($node_list as $item){

		$week_begin = strtotime($item['week_begin']);
		$week_end = strtotime($item['week_end']);
		$week_begin = format_date($week_begin, 'long');
		$week_end = format_date($week_end, 'long');

		foreach($item as $node){
			if(is_object($node)){

			$result = db_select('pressrelease_record', 'p')
				->fields('p')
				->condition('eid', $node->nid)
				->execute()
				->fetchAssoc();
				
				if($result) {
					//dsm('highlight this record: '.$node->title);
					//dsm($result);
					//$date_sent = date('m/d/Y', $result['date_sent']);
					$date_sent = format_date($result['date_sent'], 'medium');
					$notification = t(' previously sent');
					//$node->title = $node->title.' - sent';
				} else {
					$date_sent = '';
					$notification = '';
				}

				$pressrelease_date = date('m/d/Y', strtotime($node->field_pressrelease_dates['und'][0]['value']));
				$options[$node->nid] = array(
					'title' => array(
						'data' => array(
							'#type' => 'link',
							'#title' => $node->title,
							'#href' => 'node/' . $node->nid,
							'#suffix' => '<span class="marker">' . $notification.'</span>',
							//'#suffix' => ' ' . theme('mark', array('type' => node_mark($node->nid, $node->changed))),
						),
					),
					'previously_sent' => $date_sent,
					'pressrelease_date' => $pressrelease_date,
					'type' => check_plain(node_type_get_name($node)),
					'author' => theme('username', array('account' => $node)),
					'status' => $node->status ? t('published') : t('not published'),
					'changed' => format_date($node->changed, 'short'),

				);
			}
			reset($result);
			if(isset($notification)){
        //reset($notification);
        $notification = '';
			}
		}
		if(empty($options)) {
			$options = array();
		}
		$form['options']['list']['title_'.$i] = array(
			'#type' => 'item',
			'#title' => t('<hr />Week: @begin to @end', array('@begin' => $week_begin, '@end' => $week_end)),
		);
		$form['options']['list'][$i] = array(
			'#type' => 'tableselect',
			'#header' => $header,
			'#options' => $options,
			'#empty' => t('No content available.'),
		);

	$i++;
	unset($options);
	unset($items);
	unset($item);
	}

  return $form;
}


// Private function to switch through the grouping options
function pressrelease_date_grouping($group_by, $range = 16) {
  $grouping = array(); // This is the array we'll return
  // Add these other options later.  For now it's only week searches  
  switch ($group_by) {
    case 'day':
      
    break;
    case 'chron':
      
    break;
    case 'month':
      
    break;

    default:
    case 'week':
      $grouping = pressrelease_date_grouping_week($group_by, $range);
    break;
  }
  return $grouping;
}


// Private function to build the week view array
function pressrelease_date_grouping_week($group_by, $range) {
	//dpm($range);
  $week_display = array(); // This is the array we'll return for display and searching
  //$title_hour = 'l F j, Y, g:i:s a'; //Replace with Drupal styles
  $title_day = 'F j, Y'; //Replace with Drupal styles

  // Number of weeks to show:
  // We want to go back for 1/3 of the total range
  $range_start = floor($range / 3);
  // ...and go forward for 2/3 of the range
  // Example: 12 week range = 4 weeks back, 8 weeks forward
  $range_end = $range - $range_start;

  // Set some default
  $now_u = REQUEST_TIME;
  $today_u = strtotime('today');
  $day_num = date('z', REQUEST_TIME);
  $month_num = date('m', REQUEST_TIME);
  $year_num = date('Y', REQUEST_TIME);
  $year_num_current = date('Y', REQUEST_TIME);
  $week_num = date('W', REQUEST_TIME);

  // Discover the week to start
  $week_num_begin = $week_num - $range_start;

  // Fix the week number for the strtotime function
  // Add a leading zero if it doesn't exist
  if($week_num_begin == 0) {
    $week_num_begin = 01;
  } elseif($week_num_begin < 0) {
    // Here we check to see if we've gone into negative numbers, which means we've gone back a year
    $week_num_begin = $week_num_begin * (-1);
    $week_num_begin = 52 - $week_num_begin; // Set the week start number
    $year_num = $year_num - 1; // Rest the year number
  }

  // Get ready to do the loop to build the array
  $i=1; // Buid main counter
  $y=1; // Build the year counter
  // This loop is where we'll actually build the array
  while($i <= $range){
    // Here I'm checking to see if we've gone over 52 weeks.
    if($week_num_begin == 52){
      $week_num_begin = 0; // We're over 52, reset the week number

      // Here I check to see if we've gone up a year (only happens if we're above week 52)
      //if($year_num_current != $year_num){
        $year_num = $year_num + 1; // Add a year
        $y = 1; // Add one to the year counter
      //}
    }
    $week_num_begin = $week_num_begin +1; // Add one to the week so that the start week is 1 and not 0
  
    // Check the length of the string, insert a zero for the strtotext function
    $date_length = strlen($week_num_begin);
    if(strlen($week_num_begin) < 2){
      $week_num_begin = '0' . $week_num_begin; // Add the preceeding zero to week numbers that are less than 10
    }

	// This little fix will ensure that we only show the last default start/end dates
	unset($week_display['current']['begin']);
	unset($week_display['current']['end']);

	// Get the current time for initial defaults
  $week_current_start = strtotime($year_num . 'W' . $week_num); // start: unix timestamp
  $week_current_end = strtotime($year_num . 'W' . $week_num . '+7 days -1 second'); // end: unix timestamp
	$week_display['current']['begin'][$week_current_start] = 'Current week began: ' . date($title_day, $week_current_start); // current start
	$week_display['current']['end'][$week_current_end] = 'Current week ends: ' . date($title_day, $week_current_end); // current end
	$week_display['current']['begin_display'] = date($title_day, $week_current_start); // current start
	$week_display['current']['end_display'] = date($title_day, $week_current_end); // current end
	//$week_display['current_display']['begin'] = date($title_day, $week_current_start); // get the week number in case we need it
	//$week_display['current_display']['end'] = date($title_day, $week_current_end); // get the week number in case we need it
	//$week_display['current_search']['begin'] = $week_current_start; // get the week number in case we need it
	//$week_display['current_search']['end'] = $week_current_end; // get the week number in case we need it

    // Now build the array range
    $week_u_start = strtotime($year_num . 'W' . $week_num_begin); // start: unix timestamp
    $week_u_end = strtotime($year_num . 'W' . $week_num_begin . '+7 days -1 second'); // end: unix timestamp
    //$week_display[$i]['display']['begin'] = format_date($week_u_start, 'long'); // Display start date in Drupal time
    //$week_display[$i]['display']['end'] = format_date($week_u_end, 'long'); // Display end date in Drupal time
    $week_display[$i]['display']['begin'] = date($title_day, $week_u_start); // Display start date in Drupal time
    $week_display[$i]['display']['end'] = date($title_day, $week_u_end); // Display end date in Drupal time
    $week_display[$i]['search']['begin'] = $week_u_start; // start date in unix timestamp
    $week_display[$i]['search']['end'] = $week_u_end; // end date in unix timestamp
    $week_display[$i]['week_num'] = $week_num_begin; // get the week number in case we need it

    $y++; // increment up year
    $i++; // increment up
  }

/*
  print '<pre>display array<br />';
  print_r($week_display);
  print '</pre>';

	if(in_array('current', $week_display)) {
			$current['current'] = $week_display['current'];
			dpm($current);
			unset($week_display['current']);
			$week_display = array_merge($week_display, $current);
		}
		*/

  return $week_display;
}




/**
 * Rework the URLs to get the full link address
 */
function pressrelease_preprocess_node(&$vars) {	
	global $base_url;

	$enabled_content = variable_get('pressrelease_content_types', array());
	//kpr($enabled_content);
	foreach($enabled_content as $value) {
		if(in_array($vars['type'], $enabled_content, TRUE)){
			$found = TRUE;
		}
	}
	if($found){
		//$vars['node_url'] = $base_url . $vars['node_url'];
		$node_url = $base_url . $vars['node_url'];
		$vars['node_url'] = $base_url . $vars['node_url'];
		//$vars['node_url'] = $base_url . '/pressrelease/ref/?path=' . $vars['elements']['#entity_type'] . '/' .  $vars['nid'];
		$vars['redirect'] = l($vars['title'], 'pressrelease/ref/', array('absolute' => TRUE, 'query' => array('path' => $vars['elements']['#entity_type'] . '/' .  $vars['nid'])));

		//$elements = $vars['elements'];

/*
	// Get absolute URLs for the title and read more links.
	if($vars['elements']['title']['0']['#markup']){
		$source = $elements['title']['0']['#markup'];
		$result = preg_replace('('.$vars['node_url'].')', $base_url . $vars['node_url'], $source);
		$vars['elements']['title']['#items']['0']['value'] = $result;
		$vars['elements']['title']['0']['#markup'] = $result;

		$elements['title']['#items']['0']['value'] = $result;
		$elements['title']['0']['#markup'] = $result;	
	}

	if($vars['elements']['node_link']['0']['#markup']){
		$source = $elements['node_link']['0']['#markup'];
		$result = preg_replace('('.$vars['node_url'].')', $base_url . $vars['node_url'], $source);
		$vars['elements']['node_link']['#items']['0']['value'] = $result;
		$vars['elements']['node_link']['0']['#markup'] = $result;	

		$elements['node_link']['#items']['0']['value'] = $result;
		$elements['node_link']['0']['#markup'] = $result;	
	}
*/
/*
//$expression = '/<a\s+[^>]*href=(\"\'??)([^\1]+)(?:\1)>(.*)<\/a>/';
//$expression = '/<a href=\"(.*)\">(.*)<\/a>/';
$expression = '/<a.*?href\s*=\s*[\'\"](.*?)[\'\"]/';
//preg_match_all($expression, $title, $title_array);
$pattern = '/<a[^<>]*?href="[^<>]*?>(.*?)<\/a>/';
$pattern = '('.$vars['node_url'].')';
//kpr($pattern);
*/


			//kpr($base_url);
			//kpr($vars['node_url']);
			//kpr($vars);
			//kpr($elements);
	}
}


?>