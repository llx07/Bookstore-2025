import type { FormItemRule } from "naive-ui";

export const userIDValidator = (rule: FormItemRule, value: any): boolean | Error =>{
  if (!value && rule.required) {
    if(rule.required)return new Error('Field cannot be empty.');
    else return true;
  }
  const regex = /^[a-zA-Z0-9_]+$/;
  if (!regex.test(value)) {
    return new Error('User ID can only contains letters, numbers and underscore.');
  }
  if (value.length > 30) {
    return new Error('The maximum length of User ID is 30 characters.');
  }
  return true;
}

export const passwordValidator = (rule: FormItemRule, value: any): boolean | Error =>{
  if (!value) {
    if(rule.required)return new Error('Field cannot be empty.');
    else return true;
  }
  
  const regex = /^[a-zA-Z0-9_]+$/;
  if (!regex.test(value)) {
    return new Error('Password can only contains letters, numbers and underscore.');
  }
  if (value.length > 30) {
    return new Error('The maximum length of password is 30 characters.');
  }
  return true;
}