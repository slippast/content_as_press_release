<?php

/**
 * Form for the Newsletter configuration.
 */
function pressrelease_index_settings_form($form, $form_state) {

  $form['intro'] = array(
    '#type' => 'item',
    '#title' => t('This is the Press Release module settings page.'),
    //'#markup' => t('@todo:<br />1 - Assign to content types.<br />2 - Add display type to content to selected content<br />3 - Add user groups interface<hr />'),
  );
  $form['enable'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enable Press Releases for Nodes'),
    '#collapsible' => FALSE,
  );
  $form['enable']['types_markup'] = array(
    '#type' => 'markup',
    '#markup' => t('Select the node types and bundles that will have "Send as Press Release" enabled.'),
  );
  $form['enable']['types_enable']['#tree'] = TRUE;
  $form['enable']['types_enable'] = array(
    '#type' => 'checkboxes',
    '#options' => node_type_get_names(),
    '#default_value' => variable_get('pressrelease_content_types', array()),
    '#description' => t('Select the content types to be used as Newsletters.'),
  );

  // Check for the Email Form module and act accordingly. Create the markup to display.
  if(module_exists('email')) {
    $markup = 'There are several ways to get email addresses in to email groups. Option 1 is to allow users to opt in on their profile page.  Option 2 is to create a "User" or "Person" content type with an <a href="http://drupal.org/project/email" target="_blank">Email Field</a>.  The third method is in the "Groups" tab, add a bulk of comma separated addresses.';
    } else {
    $markup = 'There are several ways to get email addresses in to email groups. Option 1 is to allow users to opt in on their profile page.  Option 2 is to create a "User" or "Person" content type with an <a href="http://drupal.org/project/email" target="_blank">Email Field</a>. <em>NOTE</em>: You must have <a href="http://drupal.org/project/email" target="_blank">Email Field</a> module installed to add this functionality. The third method is in the "Groups" tab, add a bulk of comma separated addresses.';
    }

  //Now show the markup we just created - @todo: make it look prettier
  $form['users_markup'] = array(
    '#title' => t('Include users and/or content types'),
    '#type' => 'item',
    '#markup' => t($markup),
  );

  // Create list of roles to limit this by.
  $form['roles_enable'] = array(
    '#type' => 'checkbox',
    '#title' => t('Option 1: Allow users to join Groups?'),
    //'#options' => drupal_map_assoc(array(t('High School'), t('Undergraduate'), t('Other'))),
    //'#options' => node_type_get_names(),
    '#default_value' => variable_get('pressrelease_roles_enable', array()),
    '#description' => t('If this option is checked users and admins editing users will see the list of Newsletter groups and have the ability to opt in.'),
  );

  $form['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('Enable for roles'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#description' => t('All roles are disabled by default, check the roles you want to include.'),

    // Expand the expand_more_info fieldset if the box is checked.
    '#states' => array(
      'visible' => array(
        ':input[name="roles_enable"]' => array('checked' => TRUE),
      ),
      'expanded' => array(
        ':input[name="roles_enable"]' => array('checked' => TRUE),
      ),
    ),
  );


  $form['roles']['role_list']['#tree'] = TRUE;
  $form['roles']['role_list'] = array(
    '#type' => 'checkboxes',
    '#options' => user_roles(),
    '#default_value' => variable_get('pressrelease_roles', array()),
    '#description' => t('Users in the selected roles can be added to a mail group.'),
  );


  if(module_exists('email')) {
    $form['content_enable'] = array(
      '#type' => 'checkbox',
      '#title' => t('Option 2: Allow content with <a href="http://drupal.org/project/email" target="_blank">Email Fields</a> to be added to Groups?'),
      '#default_value' => variable_get('pressrelease_content_enable', array()),
      '#description' => t('If this option is checked admins creating or editing groups can add content with an <a href="http://drupal.org/project/email" target="_blank">Email Field</a> to the group.'),
    );

    $form['types'] = array(
      '#type' => 'fieldset',
      '#title' => t('Enable for content types'),
      '#collapsible' => TRUE,
      '#collapsed' => FALSE,
      '#description' => t('Allow content of these types to be added to email groups.'),
      // Expand the expand_more_info fieldset if the box is checked.
      '#states' => array(
        'visible' => array(
          ':input[name="content_enable"]' => array('checked' => TRUE),
        ),
        'expanded' => array(
          ':input[name="content_enable"]' => array('checked' => TRUE),
        ),
      ),
    );
    $form['types']['content_list']['#tree'] = TRUE;
    $form['types']['content_list'] = array(
      '#type' => 'checkboxes',
      '#options' => newsletter_get_content_types(),
      '#default_value' => variable_get('pressrelease_content', array()),
      '#description' => t('Administrators will be able to add individual pieces of content from these entity types to be added to email groups.'),
    );
  }


  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Changes')
    );

//kpr($form_state['values']);
//kpr($form_state['values']);

  return $form;
}



/**
 * Submit handler for the bundle configuration form.
 */
function pressrelease_index_settings_form_submit($form, &$form_state) {
  $values = $form_state['values'];
  //drupal_set_message('<pre>'.print_r($values, TRUE).'</pre>');

  variable_set('pressrelease_roles_enable', $values['roles_enable']);
  if (intval($values['role_list'])) {
    variable_set('pressrelease_roles', $values['role_list']);
  }

  variable_set('pressrelease_content_enable', $values['content_enable']);
  if (intval($values['content_list'])) {
    variable_set('pressrelease_content', $values['content_list']);
  }

  if (intval($values['types_enable'])) {
    variable_set('pressrelease_content_types', $values['types_enable']);
  }

foreach($values['types_enable'] as $disabled_bundle => $bundle){
  $field_name = 'field_pressrelease_dates';
  $entity_type = 'node';
  
  //kpr($bundle);
  if($bundle != '0'){
    // Create the instance on the bundle.
    if(!field_info_instance($entity_type, $field_name, $bundle)){
      $instance = array(
        'field_name' =>  $field_name,
        'entity_type' => $entity_type,
        'label' => t('Press Release Dates'),
        'bundle' => $bundle,
        //'required' => TRUE,
        'widget' => array(
          'type' => 'date_popup',
          'date_format' => 'd/m/Y',
        ),
      );
      //kpr($instance);
      field_create_instance($instance);
       drupal_set_message(t("Press Release Date fields added to content type: @bundle.", array('@bundle' => $bundle)));      
    }
  } else {
    //dsm('Disabled: '.$disabled_bundle);
    if(field_info_instance($entity_type, $field_name, $disabled_bundle)){
      $instance = array(
        'field_name' =>  $field_name,
        'entity_type' => $entity_type,
        'label' => t('Press Release Dates'),
        'bundle' => $disabled_bundle,
        'widget' => array(
          'type' => 'date_popup',
          'date_format' => 'd/m/Y',
        ),
      );
      if(field_delete_instance($instance, FALSE)){
        drupal_set_message(t("Press Release Date field removed from content type: @bundle.", array('@bundle' => $disabled_bundle)));
      }
    }
  }

  }
  drupal_set_message(t('Your configuration has been saved.'));
}

